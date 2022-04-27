### boolector
- Bug type: use-after-free
- CVE ID: [pending](https://github.com/Boolector/boolector/issues/41)
- Download: https://github.com/Boolector/boolector
```
git clone https://github.com/Boolector/boolector.git
git checkout 0874a185cd98711b3e4a0b1a0c10e858ff4a23e6
```
- Compile: `CC=wllvm CXX=wllvm++ CFLAGS="-g -O0 -pthread -Wall" CXXFLAGS="-g -O0 -pthread -Wall" LDFLAGS="-lpthread -lrt -ldl" ./configure.sh`
- Reproduce: `boolector $POC`
- ASAN:
```
=================================================================
==31295==ERROR: AddressSanitizer: heap-use-after-free on address 0x60300000e0b0 at pc 0x7f17e08cbb00 bp 0x7ffe777fdf20 sp 0x7ffe777fdf18
READ of size 8 at 0x60300000e0b0 thread T0
    #0 0x7f17e08cbaff in car /boolector/src/parser/btorsmt.c:236:16
    #1 0x7f17e08cbaff in recursively_delete_smt_node /boolector/src/parser/btorsmt.c:383
    #2 0x7f17e08ca6c3 in release_smt_nodes /boolector/src/parser/btorsmt.c:443:5
    #3 0x7f17e08ca6c3 in release_smt_internals /boolector/src/parser/btorsmt.c:449
    #4 0x7f17e08c1de1 in parse_smt_parser /boolector/src/parser/btorsmt.c:2956:3
    #5 0x7f17e0674ff1 in parse_aux /boolector/src/btorparse.c:68:15
    #6 0x7f17e0673e81 in btor_parse /boolector/src/btorparse.c:230:9
    #7 0x4f5138 in boolector_main /boolector/src/btormain.c:1462:19
    #8 0x7f17df44882f in __libc_start_main /build/glibc-Cl5G7W/glibc-2.23/csu/../csu/libc-start.c:291
    #9 0x419c58 in _start (/boolector/build/bin/boolector+0x419c58)

0x60300000e0b0 is located 0 bytes inside of 24-byte region [0x60300000e0b0,0x60300000e0c8)
freed by thread T0 here:
    #0 0x4b9c00 in __interceptor_cfree.localalias.0 (/boolector/build/bin/boolector+0x4b9c00)
    #1 0x7f17e08cc2e7 in recursively_delete_smt_node /boolector/src/parser/btorsmt.c:394:5
    #2 0x7f17e08ca6c3 in release_smt_nodes /boolector/src/parser/btorsmt.c:443:5
    #3 0x7f17e08ca6c3 in release_smt_internals /boolector/src/parser/btorsmt.c:449
    #4 0x7f17e08c1de1 in parse_smt_parser /boolector/src/parser/btorsmt.c:2956:3
    #5 0x7f17e0674ff1 in parse_aux /boolector/src/btorparse.c:68:15
    #6 0x7f17e0673e81 in btor_parse /boolector/src/btorparse.c:230:9
    #7 0x4f5138 in boolector_main /boolector/src/btormain.c:1462:19
    #8 0x7f17df44882f in __libc_start_main /build/glibc-Cl5G7W/glibc-2.23/csu/../csu/libc-start.c:291

previously allocated by thread T0 here:
    #0 0x4b9d88 in __interceptor_malloc (/boolector/build/bin/boolector+0x4b9d88)
    #1 0x7f17e09f1d6b in btor_mem_malloc /boolector/src/utils/btormem.c:75:12
    #2 0x7f17e08c151f in cons /boolector/src/parser/btorsmt.c:255:3
    #3 0x7f17e08c151f in parse /boolector/src/parser/btorsmt.c:2889
    #4 0x7f17e08c151f in parse_smt_parser /boolector/src/parser/btorsmt.c:2955
    #5 0x7f17e0674ff1 in parse_aux /boolector/src/btorparse.c:68:15
    #6 0x7f17e0673e81 in btor_parse /boolector/src/btorparse.c:230:9
    #7 0x4f5138 in boolector_main /boolector/src/btormain.c:1462:19
    #8 0x7f17df44882f in __libc_start_main /build/glibc-Cl5G7W/glibc-2.23/csu/../csu/libc-start.c:291

SUMMARY: AddressSanitizer: heap-use-after-free /boolector/src/parser/btorsmt.c:236:16 in car
Shadow bytes around the buggy address:
  0x0c067fff9bc0: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c067fff9bd0: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c067fff9be0: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c067fff9bf0: fa fa fa fa fa fa fa fa 00 00 00 fa fa fa 00 00
  0x0c067fff9c00: 00 fa fa fa fd fd fd fd fa fa 00 00 00 fa fa fa
=>0x0c067fff9c10: fd fd fd fa fa fa[fd]fd fd fa fa fa fd fd fd fd
  0x0c067fff9c20: fa fa 00 00 00 05 fa fa 00 00 07 fa fa fa fd fd
  0x0c067fff9c30: fd fd fa fa 00 00 00 00 fa fa fd fd fd fd fa fa
  0x0c067fff9c40: 00 00 00 00 fa fa 00 00 00 00 fa fa 00 00 00 00
  0x0c067fff9c50: fa fa 00 00 00 00 fa fa 00 00 00 00 fa fa 00 00
  0x0c067fff9c60: 00 00 fa fa fd fd fd fd fa fa fd fd fd fd fa fa
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07 
  Heap left redzone:       fa
  Heap right redzone:      fb
  Freed heap region:       fd
  Stack left redzone:      f1
  Stack mid redzone:       f2
  Stack right redzone:     f3
  Stack partial redzone:   f4
  Stack after return:      f5
  Stack use after scope:   f8
  Global redzone:          f9
  Global init order:       f6
  Poisoned by user:        f7
  Container overflow:      fc
  Array cookie:            ac
  Intra object redzone:    bb
  ASan internal:           fe
  Left alloca redzone:     ca
  Right alloca redzone:    cb
==31295==ABORTING
```
