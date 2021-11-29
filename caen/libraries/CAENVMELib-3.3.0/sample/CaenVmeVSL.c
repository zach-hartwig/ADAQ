/*
    ---------------------------------------------------------------------------

                   --- CAEN SpA - Front End Electronics  ---

    ---------------------------------------------------------------------------

    Name        :   CaenVmeVSL.c

    Project     :   CaenVmeVSL

    Description :   VME Scripting Language Intepreter

    Date        :   March 2006
    Release     :   1.0
    Author      :   C.Tintori

    ---------------------------------------------------------------------------


    ---------------------------------------------------------------------------
*/

/*

  Revision History:

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef LINUX
    #include <ncurses.h>
    #include <unistd.h>
    #define stricmp strcasecmp  // Compare strings without care of case
#else
    #include <conio.h>
    #include <stdarg.h>
#endif

#include "CAENVMElib.h"
#include "CAENVMEtypes.h"
#include "console.h"


#define RELEASE        "1.0"
#define RELEASE_DATE   "16 March 2006"

// Some size definitions
#define MAX_LINE_LENGHT   1000
#define MAX_ALIAS         100
#define MAX_VARS          100
#define MAX_LABEL         100
#define MAX_WORD_LENGHT   30


// Some type definitions
#ifndef U32
#define U32 uint32_t
#endif
#ifndef U16
#define U16 uint16_t
#endif
#ifndef U8
#define U8 uint8_t
#endif


// ----------------------------------------------------------------------------
// Commands Definition
// ----------------------------------------------------------------------------
// Struct of the Command
typedef struct command_type_def {
    int index;
    char name[20];
} command_type;

// Set of commands
static command_type commands[] = {
	{ 0,  "Alias",},
    { 1,  "Write",},
    { 2,  "Read",},
    { 3,  "Wait",},
    { 4,  "Goto",},
    { 5,  "Repeat",},
    { 6,  "Until",},
    { 7,  "Var",},
    { 8,  "Print",},
    { 9,  "Pause",},
    { 10, "If",},
    { 11, "Else",},
    { 12, "Echo",},
    { 13, "Log",},
    { 14, "About",},
    { 15, "WriteReg",},
    { 16, "ReadReg",},
    { 17, "Return",},
};


// ----------------------------------------------------------------------------
// Other Global Variables
// ----------------------------------------------------------------------------
static char vars[MAX_VARS][MAX_WORD_LENGHT];
static int vars_value[MAX_VARS];
static int var_n = 0;
static int enecho = 0;
static int do_log = 0;
static FILE *logfile;


// ----------------------------------------------------------------------------
// Local Functions
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// LOG messages
static void LOG(char *fmt,...)
{

    va_list marker;
    char buf[256];

    va_start(marker, fmt);
    vsprintf(buf,fmt,marker);
    va_end(marker);

#ifdef LINUX
    if (enecho)
        wprintw(stdscr, buf);
#else
    fflush(stdout);
    if (enecho)
        _cprintf(buf);
#endif
    if (do_log)
        fprintf(logfile, buf);

}

// ----------------------------------------------------------------------------
// Read a decimal or hex value or variable from a line, removing the
// characters that have been read.
static int ReadValue(char *line, int *v)
{
    char temp[100]="";
    char *c;
    int i, rs;

    sscanf(line, "%s", temp);
    if (strlen(temp) == 0)
        return -1;

    // Search for variables
    for(i=0; i<var_n; i++) {
        if(strcmp(temp, vars[i]) == 0 ) {
            *v = vars_value[i];
            strcpy(temp, strstr(line, temp)+strlen(temp));
            strcpy(line, temp);
            return 0;
        }
    }

    if ((c = strstr(temp, "0x")) != NULL)
        rs=sscanf(c+2, "%x", v);
    else
        rs=sscanf(line, "%d", v);
    strcpy(temp, strstr(line, temp)+strlen(temp));
    strcpy(line, temp);
    if (rs != -1)
        return 0;
    else
        return -1;
}

// ----------------------------------------------------------------------------
// read a string from a line removing the characters that have been read.
static int ReadString(char *line, char *s)
{
    char temp[100];

    s[0]=0;
    sscanf(line, "%s", s);
    if (strlen(s) > 0) {
        strcpy(temp, strstr(line, s)+strlen(s));
        strcpy(line, temp);
        return 0;
    }
    else
        return -1;
}

// ----------------------------------------------------------------------------
// read a condition from line the characters that have been read.
static int ReadCondition(char *line, int *r)
{
    char s1[100], temp[100], s[5];
    char *c;
    int v1, v2, ret=0;

    if ((c = strchr(line, '(')) == NULL)
        return -1;
    strcpy(s1, c+1);
    if ((c = strchr(line, ')')) == NULL)
        return -1;
    strcpy(temp, c+1);
    strcpy(line, temp);

    ret |= ReadValue(s1, &v1);
    ret |= ReadString(s1, s);
    ret |= ReadValue(s1, &v2);
    if (ret != 0)
        return -1;
    ret = 0;
    if (strcmp(s, "==") == 0)
        *r = (v1 == v2);
    else if (strcmp(s, "<=") == 0)
        *r = (v1 <= v2);
    else if (strcmp(s, ">=") == 0)
        *r = (v1 >= v2);
    else if (strcmp(s, "<") == 0)
        *r = (v1 < v2);
    else if (strcmp(s, ">") == 0)
        *r = (v1 > v2);
    else if (strcmp(s, "!=") == 0)
        *r = (v1 != v2);
    else
        ret = -1;
    return ret;
}



// ############################################################################
// ----------------------------------------------------------------------------
// CaenVmeVSL
// ----------------------------------------------------------------------------
// ############################################################################

void CaenVmeVSL(long handle, FILE *fin, int BaseAddress)
{
    char line[MAX_LINE_LENGHT];
    char temp[MAX_LINE_LENGHT];
    char cmd[MAX_WORD_LENGHT];
    char alias_sin[MAX_ALIAS][MAX_WORD_LENGHT];
    char alias_sout[MAX_ALIAS][MAX_WORD_LENGHT];
    char label[MAX_LABEL][MAX_WORD_LENGHT];
    char labeltogo[MAX_WORD_LENGHT];
    long label_pos[MAX_LABEL];
    int label_ln[MAX_LABEL];
    char *cp;
    int i, j, v, v1, res, linenum=0, cond;
    int cmd_n, found;
    int alias_n = 0;
    int label_n = 0;
    int sfgoto = 0;
    int logopen = 0;
    int rl, retl;
    long rp, retp;
    int tmp_ad, tmp_dt;
    int vme_am, vme_dw;
    uint32_t data32;
    uint16_t data16;

    // Number of commands:
    cmd_n = sizeof(commands)/sizeof(commands[0]);

    // Initializes predefined variables
    strcpy(vars[0], "Rdata");
    strcpy(vars[1], "BaseAddress");
    strcpy(vars[2], "Result");
    vars_value[0] = 0;
    vars_value[1] = BaseAddress;  // Passed as input parameter
    vars_value[2] = 0;
    var_n = 3;

    while(!feof(fin)) {
        fgets(line, MAX_LINE_LENGHT, fin);  // read one line from the vsl file
        linenum++;

        // Remove Comments
        if( (cp = strchr(line, '#')) != NULL )
            line[cp-line] = 0;

        // Replace Alias
        for(i=0; i<alias_n; i++) {
            if( (cp = strstr(line, alias_sout[i])) != NULL ) {
                temp[0] = 0;
                strncat(temp, line, cp-line);
                strcat(temp, alias_sin[i]);
                strcat(temp, cp+strlen(alias_sout[i]));
                strcpy(line, temp);
            }
        }


read_command:  // Read and execute a Command
        if(ReadString(line, cmd) == 0) {
            res = 0;
            found = 0;

            // Search for Labels
            if (cmd[strlen(cmd)-1] == ':') {
                found = 1;
                temp[0]=0;
                strncat(temp, cmd, strlen(cmd)-1);
                // Check if already existing
                for (i=0; i<label_n; i++)
                    if(stricmp(label[i], temp) == 0)
                        break;
                if (i==label_n) {
                    strcpy(label[label_n], temp);
                    label_pos[label_n] = ftell(fin);
                    label_ln[label_n] = linenum;
                    if (sfgoto && (stricmp(label[label_n], labeltogo) == 0))
                        sfgoto = 0;
                    label_n++;
                }
            }
            if (sfgoto)
                found = 1;


            // Search for variable assignements
            for(i=0; i<var_n && !found; i++) {
                if(stricmp(vars[i], cmd) == 0) {
                    temp[0]=0;
                    found = 1;
                    res |= ReadString(line, temp);
                    res |= ReadValue(line, &v);
                    if (res == 0) {
                        if (strcmp(temp, "=") == 0)
                            vars_value[i] = v;
                        else if (strcmp(temp, "+=") == 0)
                            vars_value[i] += v;
                        else if (strcmp(temp, "-=") == 0)
                            vars_value[i] -= v;
                        else if (strcmp(temp, "*=") == 0)
                            vars_value[i] *= v;
                        else if (strcmp(temp, "/=") == 0)
                            vars_value[i] /= v;
                        else if (strcmp(temp, "&=") == 0)
                            vars_value[i] &= v;
                        else if (strcmp(temp, "|=") == 0)
                            vars_value[i] |= v;
                        else if (strcmp(temp, "^=") == 0)
                            vars_value[i] ^= v;
                        else if (strcmp(temp, "<<=") == 0)
                            vars_value[i] <<= v;
                        else if (strcmp(temp, ">>=") == 0)
                            vars_value[i] >>= v;
                        else if (strcmp(temp, "%=") == 0)
                            vars_value[i] %= v;
                        else
                            LOG("Error at line %d: Bad Assignment Operator\n", linenum);

                    } else
                        LOG("Error at line %d: Bad Expression\n", linenum);
                }
            }

            // Execute Commands
            for(i=0; i<cmd_n && !found; i++) {
                if(stricmp(commands[i].name, cmd) == 0) {
                    found = 1;
                    switch(i) {
                    case 0: // Alias
                        res |= ReadString(line, alias_sout[alias_n]);
                        res |= ReadString(line, alias_sin[alias_n]);
                        if (res == 0)
                            alias_n++;
                        else
                            LOG("Error at line %d - %s: Bad Strings\n", linenum, cmd);
                        break;
                    case 1: // Write
                        res |= ReadValue(line, &vme_dw);
                        res |= ReadValue(line, &vme_am);
                        res |= ReadValue(line, &tmp_ad);
                        res |= ReadValue(line, &tmp_dt);
                        if (res == 0) {
                            if (vme_dw == cvD16) {
                                data16 = (U16)tmp_dt;
                                vars_value[2] = CAENVME_WriteCycle(handle, vars_value[1] + (U32)tmp_ad, &data16, (U8)vme_am, (U8)vme_dw);
                            } else {
                                data32 = (U32)tmp_dt;
                                vars_value[2] = CAENVME_WriteCycle(handle, vars_value[1] + (U32)tmp_ad, &data32, (U8)vme_am, (U8)vme_dw);
                            }
                            LOG("%s %d 0x%02X 0x%08X 0x%08X. Res = %d\n", cmd, vme_dw, vme_am, vars_value[1] + (U32)tmp_ad, (U32)tmp_dt, vars_value[2]);
                        }
                        else
                            LOG("Error at line %d - %s: Bad Parameters\n", linenum, cmd);
                        break;
                    case 2: // Read
                        res |= ReadValue(line, &vme_dw);
                        res |= ReadValue(line, &vme_am);
                        res |= ReadValue(line, &tmp_ad);
                        if (res == 0) {
                            if (vme_dw == cvD16) {
                                vars_value[2] = CAENVME_ReadCycle(handle, vars_value[1] + (U32)tmp_ad, &data16, (U8)vme_am, (U8)vme_dw);
                                vars_value[0] = (int)data16;
                            } else {
                                vars_value[2] = CAENVME_ReadCycle(handle, vars_value[1] + (U32)tmp_ad, &data32, (U8)vme_am, (U8)vme_dw);
                                vars_value[0] = (int)data32;
                            }
                            LOG("%s %d 0x%02X 0x%08X. VMEData = %08X. Res = %d\n", cmd, vme_dw, vme_am, vars_value[1] + (U32)tmp_ad, (U32)vars_value[0], vars_value[2]);
                        }
                        else
                            LOG("Error at line %d - %s: Bad Parameters\n", linenum, cmd);
                        break;
                    case 3: // Wait
                        res |= ReadValue(line, &v);
                        if (res == 0)
                            delay(v);
                        else
                            LOG("Error at line %d - %s: Bad Parameter\n", linenum, cmd);
                        break;
                    case 4: // Goto
                        res |= ReadString(line, labeltogo);
                        if (res == 0) {
                            for(i=0; i<label_n; i++)
                                if(strcmp(labeltogo, label[i]) == 0) {
                                    retp = ftell(fin);
                                    retl = linenum;
                                    fseek(fin, label_pos[i], SEEK_SET);
                                    linenum = label_ln[i];
                                    break;
                                }
                            if (i == label_n)  // Label Not Found. Search foreward...
                                sfgoto = 1;
                        } else
                            LOG("Error at line %d - %s: Bad Parameter\n", linenum, cmd);
                        break;
                    case 5: // Repeat
                        rp = ftell(fin);
                        rl = linenum;
                        break;
                    case 6: // Until
                        res |= ReadCondition(line, &cond);
                        if (res == 0) {
                            if (cond) {
                                fseek(fin, rp, SEEK_SET);
                                linenum = rl;
                            }
                        }
                        else
                            LOG("Error at line %d - %s: Bad Condition\n", linenum, cmd);
                        break;
                    case 7: // Var
                        res |= ReadString(line, vars[var_n]);
                        if (res == 0)
                            var_n++;
                        else
                            LOG("Error at line %d - %s: Bad Variable Name\n", linenum, cmd);
                        break;
                    case 8: // Print
                        for(i=0; line[i] != '"' && (i < MAX_LINE_LENGHT); i++);
                            if (i==MAX_LINE_LENGHT)
                                LOG("Error at line %d - %s: Missing string\n", cmd, linenum);
                            else {
                                for(j=i+1; line[j] != '"' && (j < MAX_LINE_LENGHT); j++)
                                    temp[j-i-1]=line[j];
                                temp[j-i-1]=0;
                                if (ReadValue(line+j+1, &v) == 0)
                                    if (ReadValue(line+j+1, &v1) == 0)
                                        LOG(temp, v, v1);
                                    else
                                        LOG(temp, v);
                                else
                                    LOG(temp);
                                LOG("\n");
                            }
                        line[0]=0;
                        break;
                    case 9: // Pause
                        con_getch();
                        break;
                    case 10: // If
                        res |= ReadCondition(line, &cond);
                        if (res == 0) {
                            if (cond)
                                goto read_command;
                        } else
                            LOG("Error at line %d - %s: Bad Condition\n", linenum, cmd);
                        break;
                    case 11: // Else
                        if (!cond)
                            goto read_command;
                        break;
                    case 12: // Echo
                        res |= ReadString(line, temp);
                        if (res == 0) {
                            if (stricmp(temp, "ON") == 0)
                                enecho = 1;
                            else if (stricmp(temp, "OFF") == 0)
                                enecho = 0;
                        } else
                            LOG("Error at line %d - %s: Bad Parameter\n", linenum, cmd);
                        break;
                    case 13: // Log
                        res |= ReadString(line, temp);
                        if (res == 0) {
                            if (stricmp(temp, "ON") == 0) {
                                do_log = 1;
                                if (!logopen) {
                                    if (ReadString(line, temp) == 0)
                                        logfile = fopen(temp, "w");
                                    else
                                        logfile = fopen("vsl.txt", "w");
                                    logopen = 1;
                                }
                            }
                            else if (stricmp(temp, "OFF") == 0)
                                do_log = 0;
                        } else
                            LOG("Error at line %d - %s: Bad Parameter\n", linenum, cmd);
                        break;
                    case 14: // About
                        LOG("CAEN SpA - VME Scripting Language\n");
                        LOG("Release %s - %s\n\n", RELEASE, RELEASE_DATE);
                        LOG("Recognized commands:\n");
                        for(i=0; i<cmd_n; i++)
                            LOG(" %2d - %s\n", i, commands[i].name);
                        break;
                    case 15: // WriteReg
                        res |= ReadValue(line, &tmp_ad);
                        res |= ReadValue(line, &tmp_dt);
                        if (res == 0) {
                            vars_value[2] = CAENVME_WriteRegister(handle, (U16)tmp_ad, (U16)tmp_dt);
                            LOG("%s 0x%04X 0x%04X. Res = %d\n", cmd, tmp_ad, tmp_dt, vars_value[2]);
                        }
                        else
                            LOG("Error at line %d - %s: Bad Parameters\n", linenum, cmd);
                        break;
                    case 16: // ReadReg
                        res |= ReadValue(line, &tmp_ad);
                        if (res == 0) {
                            vars_value[2] = CAENVME_ReadRegister(handle, (U16)tmp_ad, &data16);
                            vars_value[0] = (int)data16;
                            LOG("%s 0x%04X: Reg Data = 0x%04X. Res = %d\n", cmd, tmp_ad, data16, vars_value[2]);
                        }
                        else
                            LOG("Error at line %d - %s: Bad Parameters\n", linenum, cmd);
                        break;
                    case 17: // Return
                        fseek(fin, retp, SEEK_SET);
                        linenum = retl;
                        break;
                    }
                }
            }
            if (!found)
                LOG("Error at line %d - %s: Unknown Command or variable\n", linenum, cmd);
        }
    }

    // If eof is reached with sfgoto=1, then a label is missing
    if (sfgoto)
        LOG("Error: Label %s not found\n", temp);
    if(logopen)
        fclose(logfile);
}





