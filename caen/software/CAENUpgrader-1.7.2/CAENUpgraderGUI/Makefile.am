guidir = /etc/CAENUpgrader
dist_gui_DATA = *.ini dist/*.jar

jlibdir = /etc/CAENUpgrader/lib
dist_jlib_DATA = dist/lib/*.jar

templatesdir = /etc/CAENUpgrader/templates
dist_templates_DATA = templates/*.txt

plldir = /etc/CAENUpgrader/PLL
dist_pll_DATA = PLL/*.rbf

udevdir = /etc/udev/rules.d
dist_udev_DATA = script/*.rules

scriptdir = $(DESTDIR)/script
dist_script_DATA = script/*

ftdidir = $(DESTDIR)/script
dist_ftdi_DATA = lib/native/*.6

dist_bin_SCRIPTS = CAENUpgraderGUI

install-exec-hook:
	udevadm control --reload-rules
	bash script/postInstall.sh
