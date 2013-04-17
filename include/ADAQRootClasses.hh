#ifndef __ADAQRootClasses_hh__ 
#define __ADAQRootClasses_hh__ 1

#include "TGNumberEntry.h"
#include "TGTextEntry.h"
#include "TGComboBox.h"
#include "TGLabel.h"
#include "TObjString.h"

// ADAQRootClassess.hh is a header file containing class declarations
// and definitions for two types of classes:
//
//   1) to hold various measurement parameters for persistance data storage
//
//   2 to create standard ROOT widgets and labels into a single
//     horizontal frame. This greatly simplifies the ROOT GUI code
//     that desires to use widgets+labels.


// Class that holds measurement parameters for persistant data storage
// into a ROOT file
class ADAQRootMeasParams : public TObject{

public:
  std::vector<double> DetectorVoltage;
  std::vector<double> DetectorCurrent;
  std::vector<double> DCOffset;
  std::vector<double> TriggerThreshold;
  std::vector<double> BaselineCalcMin;
  std::vector<double> BaselineCalcMax;

  int RecordLength;

  ClassDef(ADAQRootMeasParams,1);
};


// Class to couple ROOT TGNumberEntry with TGLabel to the right
class ADAQNumberEntryWithLabel : public TGHorizontalFrame
{
protected:
  TGNumberEntry *fEntry;
  
public:
  ADAQNumberEntryWithLabel(const TGWindow *frame, const char *label, int id) : TGHorizontalFrame(frame)
  {
    fEntry = new TGNumberEntry(this, 0, 8, id, TGNumberFormat::kNESInteger);
    fEntry->Resize(70,20);
    fEntry->ChangeOptions(fEntry->GetOptions() | kFixedSize);
    AddFrame(fEntry, new TGLayoutHints(kLHintsLeft));
    AddFrame(new TGLabel(this,label), new TGLayoutHints(kLHintsCenterY,5,5,5,5));
  }
  TGNumberEntry *GetEntry() const { return fEntry; }
};


// Class to couple ROOT TGNumberEntryField with TGLabel to the right
class ADAQNumberEntryFieldWithLabel : public TGHorizontalFrame
{
protected:
  TGNumberEntryField *fEntry;
  
public:
  ADAQNumberEntryFieldWithLabel(const TGWindow *frame, const char *label, int id) : TGHorizontalFrame(frame)
  {
    fEntry = new TGNumberEntryField(this, id, 0, TGNumberFormat::kNESInteger);
    fEntry->Resize(70,20);
    fEntry->ChangeOptions(fEntry->GetOptions() | kFixedSize);
    AddFrame(fEntry, new TGLayoutHints(kLHintsLeft));
    AddFrame(new TGLabel(this,label), new TGLayoutHints(kLHintsCenterY,5,5,5,5));
  }
  TGNumberEntryField *GetEntry() const { return fEntry; }
};


// Class to couple ROOT TGComboBox with TGLabel to the right
class ADAQComboBoxWithLabel : public TGHorizontalFrame {

protected:
  TGComboBox *fComboBox;

public:
  ADAQComboBoxWithLabel(const TGWindow *frame, const char *label, int id) : TGHorizontalFrame(frame)
  {
    fComboBox = new TGComboBox(this, id); 
    fComboBox->Resize(100,20);
    fComboBox->ChangeOptions(fComboBox->GetOptions() | kFixedSize | kSunkenFrame);
    AddFrame(fComboBox, new TGLayoutHints(kLHintsLeft));
    AddFrame(new TGLabel(this,label), new TGLayoutHints(kLHintsCenterY,7,5,3,5));
  }
  TGComboBox *GetComboBox() const {return fComboBox;}
};


// Class to couple TGTextEntry with TGLabel to the right
class ADAQTextEntryWithLabel : public TGHorizontalFrame {

protected:
  TGTextEntry *fTextEntry;

public:
  ADAQTextEntryWithLabel(const TGWindow *frame, const char *label, int id) : TGHorizontalFrame(frame)
  {
    fTextEntry = new TGTextEntry(this,"", id);
    fTextEntry->Resize(150,20);
    fTextEntry->ChangeOptions(fTextEntry->GetOptions() | kFixedSize | kSunkenFrame);
    AddFrame(fTextEntry, new TGLayoutHints(kLHintsLeft));
    AddFrame(new TGLabel(this,label), new TGLayoutHints(kLHintsCenterY,7,5,3,5));
  }
  TGTextEntry *GetEntry() const {return fTextEntry;}
};

#endif


