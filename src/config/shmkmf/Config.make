# :*: create Makefile from shell
# :de: Makefile mithilfe der Shell generieren

VN=${EFEU_VERSION:-shmkmf 1.0}

mf_srclist
mf_dir $TOP/etc/shmkmf
foreach -p $TOP/bin -s sh 'mf_file -x -s "/:VN:/$VN/g" $tg $src'
foreach -p $TOP/lib/shmkmf -S smh 'mf_file $tg $src'
foreach -p $TOP/lib/shmkmf -S msg 'mf_file $tg $src'
foreach -p $TOP/lib/shmkmf -S awk 'mf_file $tg $src'
foreach -p $TOP/lib/efeu -M de -x help -S hlp 'mf_file $tg $src'

include efeudoc.smh
efeudoc_setup
