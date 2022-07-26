### NASM 2.14rc0
- Bug type: use-after-free
- CVE ID: [CVE-2019-8343](https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2019-8343), [CVE-2018-20535](https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2018-20535), [CVE-2018-20538](https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2018-20538), [POC-CVE-2018-19216](https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2018-19216), [CVE-2017-17820](https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2017-17820), [CVE-2017-17817](https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2017-17817), [CVE-2017-17816](https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2017-17816), [CVE-2017-17814](https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2017-17816), [CVE-2017-17813](https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2017-17816)
- Download: 
  ```
  git clone git://repo.or.cz/nasm.git
  git checkout 7a81ead31b40392e9b0dff811ac0d3d4653ccfef
  ```
- Install:
  ```
  CC=afl-clang-fast CXX=afl-clang-fast++ CFLAGS="-g -O0 -fsanitize=address" CXXFLAGS="-g -O0 -fsanitize=address" LDFLAGS="-fsanitize=address" ./configure --prefix=$PWD/build

  make

  make install
  ```
- Reproduce: `./nasm -f bin $POC -o ./tmp`
- ASAN dumps the backtrace:

**POC-CVE-2019-8343**
```
=================================================================
==19325==ERROR: AddressSanitizer: heap-use-after-free on address 0x602000008c50 at pc 0x000000436af3 bp 0x7ffdf7aed380 sp 0x7ffdf7aecb30
READ of size 2 at 0x602000008c50 thread T0
    #0 0x436af2 in __interceptor_strlen.part.45 (/home/hjwang/UAF_Object/nasm_asan/build/bin/nasm+0x436af2)
    #1 0x54bdc1 in paste_tokens /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:3806:20
    #2 0x546b14 in expand_smacro /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:4475:13
    #3 0x529e95 in pp_getline /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:5210:21
    #4 0x4edc74 in assemble_file /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:1233:24
    #5 0x4eb336 in main /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:453:9
    #6 0x7fdae260a82f in __libc_start_main /build/glibc-LK5gWL/glibc-2.23/csu/../csu/libc-start.c:291
    #7 0x418f18 in _start (/home/hjwang/UAF_Object/nasm_asan/build/bin/nasm+0x418f18)

0x602000008c50 is located 0 bytes inside of 3-byte region [0x602000008c50,0x602000008c53)
freed by thread T0 here:
    #0 0x4b8ec0 in __interceptor_cfree.localalias.0 (/home/hjwang/UAF_Object/nasm_asan/build/bin/nasm+0x4b8ec0)
    #1 0x4f416f in nasm_free /home/hjwang/UAF_Object/nasm_asan/nasmlib/malloc.c:77:9
    #2 0x52c600 in delete_Token /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:1236:5
    #3 0x54ba42 in paste_tokens /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:3766:20
    #4 0x546b14 in expand_smacro /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:4475:13
    #5 0x529e95 in pp_getline /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:5210:21
    #6 0x4edc74 in assemble_file /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:1233:24
    #7 0x4eb336 in main /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:453:9
    #8 0x7fdae260a82f in __libc_start_main /build/glibc-LK5gWL/glibc-2.23/csu/../csu/libc-start.c:291

previously allocated by thread T0 here:
    #0 0x4b9048 in __interceptor_malloc (/home/hjwang/UAF_Object/nasm_asan/build/bin/nasm+0x4b9048)
    #1 0x4f4004 in nasm_malloc /home/hjwang/UAF_Object/nasm_asan/nasmlib/malloc.c:47:15
    #2 0x52cff0 in new_Token /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:1226:19
    #3 0x5340e3 in tokenize /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:1148:25
    #4 0x54bf08 in paste_tokens /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:3816:19
    #5 0x546b14 in expand_smacro /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:4475:13
    #6 0x529e95 in pp_getline /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:5210:21
    #7 0x4edc74 in assemble_file /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:1233:24
    #8 0x4eb336 in main /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:453:9
    #9 0x7fdae260a82f in __libc_start_main /build/glibc-LK5gWL/glibc-2.23/csu/../csu/libc-start.c:291

SUMMARY: AddressSanitizer: heap-use-after-free (/home/hjwang/UAF_Object/nasm_asan/build/bin/nasm+0x436af2) in __interceptor_strlen.part.45
Shadow bytes around the buggy address:
  0x0c047fff9130: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c047fff9140: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c047fff9150: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c047fff9160: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c047fff9170: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
=>0x0c047fff9180: fa fa fa fa fa fa 02 fa fa fa[fd]fa fa fa fd fa
  0x0c047fff9190: fa fa fd fa fa fa 03 fa fa fa 02 fa fa fa 02 fa
  0x0c047fff91a0: fa fa 02 fa fa fa fd fd fa fa fd fa fa fa fd fa
  0x0c047fff91b0: fa fa fd fa fa fa fd fd fa fa fd fa fa fa 00 07
  0x0c047fff91c0: fa fa 00 07 fa fa 00 05 fa fa 00 05 fa fa 00 05
  0x0c047fff91d0: fa fa 00 05 fa fa 00 05 fa fa 00 05 fa fa 00 01
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
==19325==ABORTING
Aborted (core dumped)
```

**POC-CVE-2018-20538**
```
=================================================================
==19442==ERROR: AddressSanitizer: heap-use-after-free on address 0x60f00000d520 at pc 0x0000005285a8 bp 0x7fff8d914370 sp 0x7fff8d914368
READ of size 8 at 0x60f00000d520 thread T0
    #0 0x5285a7 in pp_getline /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:5024:31
    #1 0x4edc74 in assemble_file /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:1233:24
    #2 0x4eb336 in main /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:453:9
    #3 0x7f71ef20782f in __libc_start_main /build/glibc-LK5gWL/glibc-2.23/csu/../csu/libc-start.c:291
    #4 0x418f18 in _start (/home/hjwang/UAF_Object/nasm_asan/build/bin/nasm+0x418f18)

0x60f00000d520 is located 16 bytes inside of 176-byte region [0x60f00000d510,0x60f00000d5c0)
freed by thread T0 here:
    #0 0x4b8ec0 in __interceptor_cfree.localalias.0 (/home/hjwang/UAF_Object/nasm_asan/build/bin/nasm+0x4b8ec0)
    #1 0x4f416f in nasm_free /home/hjwang/UAF_Object/nasm_asan/nasmlib/malloc.c:77:9
    #2 0x52d6fe in free_mmacro /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:636:5
    #3 0x53ca48 in do_directive /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:2955:17
    #4 0x529abf in pp_getline /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:5172:13
    #5 0x4edc74 in assemble_file /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:1233:24
    #6 0x4eb336 in main /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:453:9
    #7 0x7f71ef20782f in __libc_start_main /build/glibc-LK5gWL/glibc-2.23/csu/../csu/libc-start.c:291

previously allocated by thread T0 here:
    #0 0x4b91d0 in calloc (/home/hjwang/UAF_Object/nasm_asan/build/bin/nasm+0x4b91d0)
    #1 0x4f405c in nasm_calloc /home/hjwang/UAF_Object/nasm_asan/nasmlib/malloc.c:55:15
    #2 0x4f40bb in nasm_zalloc /home/hjwang/UAF_Object/nasm_asan/nasmlib/malloc.c:63:12
    #3 0x53b731 in do_directive /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:2868:20
    #4 0x529abf in pp_getline /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:5172:13
    #5 0x4edc74 in assemble_file /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:1233:24
    #6 0x4eb336 in main /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:453:9
    #7 0x7f71ef20782f in __libc_start_main /build/glibc-LK5gWL/glibc-2.23/csu/../csu/libc-start.c:291

SUMMARY: AddressSanitizer: heap-use-after-free /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:5024:31 in pp_getline
Shadow bytes around the buggy address:
  0x0c1e7fff9a50: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c1e7fff9a60: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c1e7fff9a70: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c1e7fff9a80: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c1e7fff9a90: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
=>0x0c1e7fff9aa0: fa fa fd fd[fd]fd fd fd fd fd fd fd fd fd fd fd
  0x0c1e7fff9ab0: fd fd fd fd fd fd fd fd fa fa fa fa fa fa fa fa
  0x0c1e7fff9ac0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c1e7fff9ad0: 00 00 00 00 00 00 fa fa fa fa fa fa fa fa 00 00
  0x0c1e7fff9ae0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c1e7fff9af0: 00 00 00 00 fa fa fa fa fa fa fa fa 00 00 00 00
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
==19442==ABORTING
Aborted (core dumped)
```

**POC-CVE-2018-20535** && **POC-CVE-2017-17816**
```
=================================================================
==19527==ERROR: AddressSanitizer: heap-use-after-free on address 0x60f00000d5a0 at pc 0x000000529261 bp 0x7ffede8e74f0 sp 0x7ffede8e74e8
READ of size 4 at 0x60f00000d5a0 thread T0
    #0 0x529260 in pp_getline /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:5112:39
    #1 0x4edc74 in assemble_file /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:1233:24
    #2 0x4eb336 in main /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:453:9
    #3 0x7f1227f8b82f in __libc_start_main /build/glibc-LK5gWL/glibc-2.23/csu/../csu/libc-start.c:291
    #4 0x418f18 in _start (/home/hjwang/UAF_Object/nasm_asan/build/bin/nasm+0x418f18)

0x60f00000d5a0 is located 144 bytes inside of 176-byte region [0x60f00000d510,0x60f00000d5c0)
freed by thread T0 here:
    #0 0x4b8ec0 in __interceptor_cfree.localalias.0 (/home/hjwang/UAF_Object/nasm_asan/build/bin/nasm+0x4b8ec0)
    #1 0x4f416f in nasm_free /home/hjwang/UAF_Object/nasm_asan/nasmlib/malloc.c:77:9
    #2 0x52d6fe in free_mmacro /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:636:5
    #3 0x53ca48 in do_directive /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:2955:17
    #4 0x529abf in pp_getline /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:5172:13
    #5 0x4edc74 in assemble_file /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:1233:24
    #6 0x4eb336 in main /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:453:9
    #7 0x7f1227f8b82f in __libc_start_main /build/glibc-LK5gWL/glibc-2.23/csu/../csu/libc-start.c:291

previously allocated by thread T0 here:
    #0 0x4b91d0 in calloc (/home/hjwang/UAF_Object/nasm_asan/build/bin/nasm+0x4b91d0)
    #1 0x4f405c in nasm_calloc /home/hjwang/UAF_Object/nasm_asan/nasmlib/malloc.c:55:15
    #2 0x4f40bb in nasm_zalloc /home/hjwang/UAF_Object/nasm_asan/nasmlib/malloc.c:63:12
    #3 0x53b731 in do_directive /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:2868:20
    #4 0x529abf in pp_getline /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:5172:13
    #5 0x4edc74 in assemble_file /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:1233:24
    #6 0x4eb336 in main /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:453:9
    #7 0x7f1227f8b82f in __libc_start_main /build/glibc-LK5gWL/glibc-2.23/csu/../csu/libc-start.c:291

SUMMARY: AddressSanitizer: heap-use-after-free /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:5112:39 in pp_getline
Shadow bytes around the buggy address:
  0x0c1e7fff9a60: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c1e7fff9a70: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c1e7fff9a80: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c1e7fff9a90: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c1e7fff9aa0: fa fa fd fd fd fd fd fd fd fd fd fd fd fd fd fd
=>0x0c1e7fff9ab0: fd fd fd fd[fd]fd fd fd fa fa fa fa fa fa fa fa
  0x0c1e7fff9ac0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c1e7fff9ad0: 00 00 00 00 00 00 fa fa fa fa fa fa fa fa 00 00
  0x0c1e7fff9ae0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c1e7fff9af0: 00 00 00 00 fa fa fa fa fa fa fa fa 00 00 00 00
  0x0c1e7fff9b00: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
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
==19527==ABORTING
Aborted (core dumped)
```

**POC-CVE-2018-19216**
```
=================================================================
==142480==ERROR: AddressSanitizer: heap-use-after-free on address 0x602000008650 at pc 0x000000533a6c bp 0x7ffe4faf41f0 sp 0x7ffe4faf41e8
READ of size 1 at 0x602000008650 thread T0
    #0 0x533a6b in detoken /home/hjwang/UAF_Object/nasm_afl_asan/asm/preproc.c:1290:13
    #1 0x52e94a in pp_getline /home/hjwang/UAF_Object/nasm_afl_asan/asm/preproc.c:5215:24
    #2 0x4ed70d in assemble_file /home/hjwang/UAF_Object/nasm_afl_asan/asm/nasm.c:1233:24
    #3 0x4ed70d in main /home/hjwang/UAF_Object/nasm_afl_asan/asm/nasm.c:453
    #4 0x7f345db7f82f in __libc_start_main /build/glibc-LK5gWL/glibc-2.23/csu/../csu/libc-start.c:291
    #5 0x4190f8 in _start (/home/hjwang/UAF_Object/nasm_afl_asan/build/bin/nasm+0x4190f8)

0x602000008650 is located 0 bytes inside of 11-byte region [0x602000008650,0x60200000865b)
freed by thread T0 here:
    #0 0x4b90a0 in __interceptor_cfree.localalias.0 (/home/hjwang/UAF_Object/nasm_afl_asan/build/bin/nasm+0x4b90a0)
    #1 0x4f4fe7 in nasm_free /home/hjwang/UAF_Object/nasm_afl_asan/nasmlib/malloc.c:77:9
    #2 0x52e94a in pp_getline /home/hjwang/UAF_Object/nasm_afl_asan/asm/preproc.c:5215:24
    #3 0x4ed70d in assemble_file /home/hjwang/UAF_Object/nasm_afl_asan/asm/nasm.c:1233:24
    #4 0x4ed70d in main /home/hjwang/UAF_Object/nasm_afl_asan/asm/nasm.c:453
    #5 0x7f345db7f82f in __libc_start_main /build/glibc-LK5gWL/glibc-2.23/csu/../csu/libc-start.c:291

previously allocated by thread T0 here:
    #0 0x4b9228 in __interceptor_malloc (/home/hjwang/UAF_Object/nasm_afl_asan/build/bin/nasm+0x4b9228)
    #1 0x4f4d57 in nasm_malloc /home/hjwang/UAF_Object/nasm_afl_asan/nasmlib/malloc.c:47:15

SUMMARY: AddressSanitizer: heap-use-after-free /home/hjwang/UAF_Object/nasm_afl_asan/asm/preproc.c:1290:13 in detoken
Shadow bytes around the buggy address:
  0x0c047fff9070: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c047fff9080: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c047fff9090: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c047fff90a0: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c047fff90b0: fa fa fa fa fa fa fa fa fa fa fa fa fa fa 00 05
=>0x0c047fff90c0: fa fa fd fd fa fa fd fd fa fa[fd]fd fa fa fd fd
  0x0c047fff90d0: fa fa 02 fa fa fa 07 fa fa fa fd fd fa fa fd fa
  0x0c047fff90e0: fa fa fd fa fa fa fd fa fa fa fd fa fa fa fd fa
  0x0c047fff90f0: fa fa fd fa fa fa fd fa fa fa fd fa fa fa fd fd
  0x0c047fff9100: fa fa fd fa fa fa fd fa fa fa fd fa fa fa fd fa
  0x0c047fff9110: fa fa fd fa fa fa fd fa fa fa fd fa fa fa fd fa
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
==142480==ABORTING
Aborted
```

**POC-CVE-2017-17820**
```
=================================================================
==19625==ERROR: AddressSanitizer: heap-use-after-free on address 0x60f00000d538 at pc 0x000000529a89 bp 0x7fff70919d90 sp 0x7fff70919d88
READ of size 8 at 0x60f00000d538 thread T0
    #0 0x529a88 in pp_getline /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:5165:45
    #1 0x4edc74 in assemble_file /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:1233:24
    #2 0x4eb336 in main /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:453:9
    #3 0x7f5bd8c4a82f in __libc_start_main /build/glibc-LK5gWL/glibc-2.23/csu/../csu/libc-start.c:291
    #4 0x418f18 in _start (/home/hjwang/UAF_Object/nasm_asan/build/bin/nasm+0x418f18)

0x60f00000d538 is located 40 bytes inside of 176-byte region [0x60f00000d510,0x60f00000d5c0)
freed by thread T0 here:
    #0 0x4b8ec0 in __interceptor_cfree.localalias.0 (/home/hjwang/UAF_Object/nasm_asan/build/bin/nasm+0x4b8ec0)
    #1 0x4f416f in nasm_free /home/hjwang/UAF_Object/nasm_asan/nasmlib/malloc.c:77:9
    #2 0x52d6fe in free_mmacro /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:636:5
    #3 0x528ff8 in pp_getline /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:5099:25
    #4 0x4edc74 in assemble_file /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:1233:24
    #5 0x4eb336 in main /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:453:9
    #6 0x7f5bd8c4a82f in __libc_start_main /build/glibc-LK5gWL/glibc-2.23/csu/../csu/libc-start.c:291

previously allocated by thread T0 here:
    #0 0x4b9048 in __interceptor_malloc (/home/hjwang/UAF_Object/nasm_asan/build/bin/nasm+0x4b9048)
    #1 0x4f4004 in nasm_malloc /home/hjwang/UAF_Object/nasm_asan/nasmlib/malloc.c:47:15
    #2 0x53d790 in do_directive /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:3054:20
    #3 0x529abf in pp_getline /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:5172:13
    #4 0x4edc74 in assemble_file /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:1233:24
    #5 0x4eb336 in main /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:453:9
    #6 0x7f5bd8c4a82f in __libc_start_main /build/glibc-LK5gWL/glibc-2.23/csu/../csu/libc-start.c:291

SUMMARY: AddressSanitizer: heap-use-after-free /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:5165:45 in pp_getline
Shadow bytes around the buggy address:
  0x0c1e7fff9a50: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c1e7fff9a60: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c1e7fff9a70: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c1e7fff9a80: fa fa fa fa fd fd fd fd fd fd fd fd fd fd fd fd
  0x0c1e7fff9a90: fd fd fd fd fd fd fd fd fd fd fa fa fa fa fa fa
=>0x0c1e7fff9aa0: fa fa fd fd fd fd fd[fd]fd fd fd fd fd fd fd fd
  0x0c1e7fff9ab0: fd fd fd fd fd fd fd fd fa fa fa fa fa fa fa fa
  0x0c1e7fff9ac0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c1e7fff9ad0: 00 00 00 00 00 00 fa fa fa fa fa fa fa fa 00 00
  0x0c1e7fff9ae0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c1e7fff9af0: 00 00 00 00 fa fa fa fa fa fa fa fa 00 00 00 00
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
==19625==ABORTING
Aborted (core dumped)
```

**POC-CVE-2017-17817**
```
=================================================================
==19712==ERROR: AddressSanitizer: heap-use-after-free on address 0x60f00000d430 at pc 0x00000052cb52 bp 0x7ffc5987b370 sp 0x7ffc5987b368
READ of size 8 at 0x60f00000d430 thread T0
    #0 0x52cb51 in pp_verror /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:4929:31
    #1 0x4fcd41 in nasm_error /home/hjwang/UAF_Object/nasm_asan/asm/error.c:86:5
    #2 0x536c3d in do_directive /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:2341:9
    #3 0x529abf in pp_getline /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:5172:13
    #4 0x4edc74 in assemble_file /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:1233:24
    #5 0x4eb336 in main /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:453:9
    #6 0x7f1f24f4b82f in __libc_start_main /build/glibc-LK5gWL/glibc-2.23/csu/../csu/libc-start.c:291
    #7 0x418f18 in _start (/home/hjwang/UAF_Object/nasm_asan/build/bin/nasm+0x418f18)

0x60f00000d430 is located 16 bytes inside of 176-byte region [0x60f00000d420,0x60f00000d4d0)
freed by thread T0 here:
    #0 0x4b8ec0 in __interceptor_cfree.localalias.0 (/home/hjwang/UAF_Object/nasm_asan/build/bin/nasm+0x4b8ec0)
    #1 0x4f416f in nasm_free /home/hjwang/UAF_Object/nasm_asan/nasmlib/malloc.c:77:9
    #2 0x52d6fe in free_mmacro /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:636:5
    #3 0x528ff8 in pp_getline /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:5099:25
    #4 0x4edc74 in assemble_file /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:1233:24
    #5 0x4eb336 in main /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:453:9
    #6 0x7f1f24f4b82f in __libc_start_main /build/glibc-LK5gWL/glibc-2.23/csu/../csu/libc-start.c:291

previously allocated by thread T0 here:
    #0 0x4b9048 in __interceptor_malloc (/home/hjwang/UAF_Object/nasm_asan/build/bin/nasm+0x4b9048)
    #1 0x4f4004 in nasm_malloc /home/hjwang/UAF_Object/nasm_asan/nasmlib/malloc.c:47:15
    #2 0x53d790 in do_directive /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:3054:20
    #3 0x529abf in pp_getline /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:5172:13
    #4 0x4edc74 in assemble_file /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:1233:24
    #5 0x4eb336 in main /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:453:9
    #6 0x7f1f24f4b82f in __libc_start_main /build/glibc-LK5gWL/glibc-2.23/csu/../csu/libc-start.c:291

SUMMARY: AddressSanitizer: heap-use-after-free /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:4929:31 in pp_verror
Shadow bytes around the buggy address:
  0x0c1e7fff9a30: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c1e7fff9a40: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c1e7fff9a50: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c1e7fff9a60: fa fa fa fa fa fa 00 00 00 00 00 00 00 00 00 00
  0x0c1e7fff9a70: 00 00 00 00 00 00 00 00 00 00 00 00 fa fa fa fa
=>0x0c1e7fff9a80: fa fa fa fa fd fd[fd]fd fd fd fd fd fd fd fd fd
  0x0c1e7fff9a90: fd fd fd fd fd fd fd fd fd fd fa fa fa fa fa fa
  0x0c1e7fff9aa0: fa fa 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c1e7fff9ab0: 00 00 00 00 00 00 00 00 fa fa fa fa fa fa fa fa
  0x0c1e7fff9ac0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c1e7fff9ad0: 00 00 00 00 00 00 fa fa fa fa fa fa fa fa 00 00
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
==19712==ABORTING
Aborted (core dumped)
```

**POC-CVE-2017-17814**
```
=================================================================
==19853==ERROR: AddressSanitizer: heap-use-after-free on address 0x60f00000cf80 at pc 0x00000053cf5b bp 0x7ffd93797a70 sp 0x7ffd93797a68
READ of size 8 at 0x60f00000cf80 thread T0
    #0 0x53cf5a in do_directive /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:2992:31
    #1 0x529abf in pp_getline /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:5172:13
    #2 0x4edc74 in assemble_file /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:1233:24
    #3 0x4eb336 in main /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:453:9
    #4 0x7f34c9f2f82f in __libc_start_main /build/glibc-LK5gWL/glibc-2.23/csu/../csu/libc-start.c:291
    #5 0x418f18 in _start (/home/hjwang/UAF_Object/nasm_asan/build/bin/nasm+0x418f18)

0x60f00000cf80 is located 16 bytes inside of 176-byte region [0x60f00000cf70,0x60f00000d020)
freed by thread T0 here:
    #0 0x4b8ec0 in __interceptor_cfree.localalias.0 (/home/hjwang/UAF_Object/nasm_asan/build/bin/nasm+0x4b8ec0)
    #1 0x4f416f in nasm_free /home/hjwang/UAF_Object/nasm_asan/nasmlib/malloc.c:77:9
    #2 0x52d6fe in free_mmacro /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:636:5
    #3 0x528ff8 in pp_getline /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:5099:25
    #4 0x4edc74 in assemble_file /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:1233:24
    #5 0x4eb336 in main /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:453:9
    #6 0x7f34c9f2f82f in __libc_start_main /build/glibc-LK5gWL/glibc-2.23/csu/../csu/libc-start.c:291

previously allocated by thread T0 here:
    #0 0x4b9048 in __interceptor_malloc (/home/hjwang/UAF_Object/nasm_asan/build/bin/nasm+0x4b9048)
    #1 0x4f4004 in nasm_malloc /home/hjwang/UAF_Object/nasm_asan/nasmlib/malloc.c:47:15
    #2 0x53d790 in do_directive /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:3054:20
    #3 0x529abf in pp_getline /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:5172:13
    #4 0x4edc74 in assemble_file /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:1233:24
    #5 0x4eb336 in main /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:453:9
    #6 0x7f34c9f2f82f in __libc_start_main /build/glibc-LK5gWL/glibc-2.23/csu/../csu/libc-start.c:291

SUMMARY: AddressSanitizer: heap-use-after-free /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:2992:31 in do_directive
Shadow bytes around the buggy address:
  0x0c1e7fff99a0: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c1e7fff99b0: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c1e7fff99c0: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c1e7fff99d0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c1e7fff99e0: 00 00 00 00 00 00 fa fa fa fa fa fa fa fa fd fd
=>0x0c1e7fff99f0:[fd]fd fd fd fd fd fd fd fd fd fd fd fd fd fd fd
  0x0c1e7fff9a00: fd fd fd fd fa fa fa fa fa fa fa fa 00 00 00 00
  0x0c1e7fff9a10: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c1e7fff9a20: 00 00 fa fa fa fa fa fa fa fa 00 00 00 00 00 00
  0x0c1e7fff9a30: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c1e7fff9a40: fa fa fa fa fa fa fa fa 00 00 00 00 00 00 00 00
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
==19853==ABORTING
Aborted (core dumped)
```

**POC-CVE-2017-17813**
```
=================================================================
==19934==ERROR: AddressSanitizer: heap-use-after-free on address 0x60f00000d568 at pc 0x000000557958 bp 0x7ffd1e386530 sp 0x7ffd1e386528
READ of size 8 at 0x60f00000d568 thread T0
    #0 0x557957 in pp_list_one_macro /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:5389:26
    #1 0x557966 in pp_list_one_macro /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:5389:5
    #2 0x52abed in pp_error_list_macros /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:5406:9
    #3 0x4f3909 in nasm_verror_common /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:1665:5
    #4 0x4eb910 in nasm_verror_gnu /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:1508:5
    #5 0x4fcd41 in nasm_error /home/hjwang/UAF_Object/nasm_asan/asm/error.c:86:5
    #6 0x51e078 in parse_line /home/hjwang/UAF_Object/nasm_asan/asm/parser.c:474:13
    #7 0x4edcd5 in assemble_file /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:1245:13
    #8 0x4eb336 in main /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:453:9
    #9 0x7ff3729c082f in __libc_start_main /build/glibc-LK5gWL/glibc-2.23/csu/../csu/libc-start.c:291
    #10 0x418f18 in _start (/home/hjwang/UAF_Object/nasm_asan/build/bin/nasm+0x418f18)

0x60f00000d568 is located 88 bytes inside of 176-byte region [0x60f00000d510,0x60f00000d5c0)
freed by thread T0 here:
    #0 0x4b8ec0 in __interceptor_cfree.localalias.0 (/home/hjwang/UAF_Object/nasm_asan/build/bin/nasm+0x4b8ec0)
    #1 0x4f416f in nasm_free /home/hjwang/UAF_Object/nasm_asan/nasmlib/malloc.c:77:9
    #2 0x52d6fe in free_mmacro /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:636:5
    #3 0x528ff8 in pp_getline /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:5099:25
    #4 0x4edc74 in assemble_file /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:1233:24
    #5 0x4eb336 in main /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:453:9
    #6 0x7ff3729c082f in __libc_start_main /build/glibc-LK5gWL/glibc-2.23/csu/../csu/libc-start.c:291

previously allocated by thread T0 here:
    #0 0x4b9048 in __interceptor_malloc (/home/hjwang/UAF_Object/nasm_asan/build/bin/nasm+0x4b9048)
    #1 0x4f4004 in nasm_malloc /home/hjwang/UAF_Object/nasm_asan/nasmlib/malloc.c:47:15
    #2 0x53d790 in do_directive /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:3054:20
    #3 0x529abf in pp_getline /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:5172:13
    #4 0x4edc74 in assemble_file /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:1233:24
    #5 0x4eb336 in main /home/hjwang/UAF_Object/nasm_asan/asm/nasm.c:453:9
    #6 0x7ff3729c082f in __libc_start_main /build/glibc-LK5gWL/glibc-2.23/csu/../csu/libc-start.c:291

SUMMARY: AddressSanitizer: heap-use-after-free /home/hjwang/UAF_Object/nasm_asan/asm/preproc.c:5389:26 in pp_list_one_macro
Shadow bytes around the buggy address:
  0x0c1e7fff9a50: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c1e7fff9a60: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c1e7fff9a70: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x0c1e7fff9a80: fa fa fa fa 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c1e7fff9a90: 00 00 00 00 00 00 00 00 00 00 fa fa fa fa fa fa
=>0x0c1e7fff9aa0: fa fa fd fd fd fd fd fd fd fd fd fd fd[fd]fd fd
  0x0c1e7fff9ab0: fd fd fd fd fd fd fd fd fa fa fa fa fa fa fa fa
  0x0c1e7fff9ac0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c1e7fff9ad0: 00 00 00 00 00 00 fa fa fa fa fa fa fa fa 00 00
  0x0c1e7fff9ae0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x0c1e7fff9af0: 00 00 00 00 fa fa fa fa fa fa fa fa 00 00 00 00
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
==19934==ABORTING
Aborted (core dumped)
```