" commands to include GPL-Licence
command! GPL read VIM/License.txt
command! SHGPL read !sed -e 's/^/\# /' VIM/License.txt
command! CGPL read VIM/License.txt
command! LGPL read VIM/LibLicense.txt
command! SHLGPL read !sed -e 's/^/\# /' VIM/LibLicense.txt

