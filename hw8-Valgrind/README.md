# Fix memory leaks using Valgrind

## Fix #1: we need to call custom deleter for 'http_get_response_t' variable, before 'goto'
### Leak backtrace
==24838== 1,128 (768 direct, 360 indirect) bytes in 24 blocks are definitely lost in loss record 80 of 143

==24838==    at 0x4C31B0F: malloc (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)

==24838==    by 0x128550: http_get_shared (http-get.c:46)

==24838==    by 0x11DDC3: clib_package_new_from_slug_with_package_name (clib-package.c:660)

==24838==    by 0x11E438: clib_package_new_from_slug (clib-package.c:796)

==24838==    by 0x11CF10: install_packages (clib-package.c:370)

==24838==    by 0x1209C1: clib_package_install_dependencies (clib-package.c:1615)

==24838==    by 0x120874: clib_package_install (clib-package.c:1579)

==24838==    by 0x11CF31: install_packages (clib-package.c:374)

==24838==    by 0x1209C1: clib_package_install_dependencies (clib-package.c:1615)

==24838==    by 0x120874: clib_package_install (clib-package.c:1579)

==24838==    by 0x11B724: main (package-install.c:84)
### Leak summary before fix :
==24933==    definitely lost: 2,554 bytes in 151 blocks

==24933==    indirectly lost: 1,020 bytes in 68 blocks

==24933==      possibly lost: 49,591,680 bytes in 337,788 blocks

==24933==    still reachable: 41,801 bytes in 12 blocks

==24933==         suppressed: 0 bytes in 0 blocks
### Leak summary after fix :
==25567==    definitely lost: 378 bytes in 83 blocks

==25567==    indirectly lost: 0 bytes in 0 blocks

==25567==      possibly lost: 49,591,680 bytes in 337,788 blocks

==25567==    still reachable: 41,801 bytes in 12 blocks

==25567==         suppressed: 0 bytes in 0 blocks



## Fix #2: pthread_exit(status) status argument must be created in another thread 
276 bytes in 69 blocks are definitely lost in loss record 42 of 111

==29529==    at 0x4C31B0F: malloc (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)

==29529==    by 0x11EC73: fetch_package_file_thread (clib-package.c:1000)

==29529==    by 0x50C56DA: start_thread (pthread_create.c:463)

==29529==    by 0x53FE71E: clone (clone.S:95)

### Leak summary before fix:
==29529==    definitely lost: 378 bytes in 83 blocks

==29529==    indirectly lost: 0 bytes in 0 blocks

==29529==      possibly lost: 49,591,680 bytes in 337,788 blocks

==29529==    still reachable: 41,801 bytes in 12 blocks

==29529==         suppressed: 0 bytes in 0 blocks

### Leak summary after fix:
==5167==    definitely lost: 102 bytes in 14 blocks

==5167==    indirectly lost: 0 bytes in 0 blocks

==5167==      possibly lost: 49,591,680 bytes in 337,788 blocks

==5167==    still reachable: 41,801 bytes in 12 blocks

==5167==         suppressed: 0 bytes in 0 blocks


## Fix #3: Clear key in hash_set if there already exist an instance with such key

==10210== 57 bytes in 7 blocks are definitely lost in loss record 25 of 110

==10210==    at 0x4C31B0F: malloc (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)

==10210==    by 0x140871: strdup (strdup.c:24)

==10210==    by 0x11FF3E: clib_package_install (clib-package.c:1382)

==10210==    by 0x11CF31: install_packages (clib-package.c:374)

==10210==    by 0x1209B0: clib_package_install_dependencies (clib-package.c:1614)

==10210==    by 0x120863: clib_package_install (clib-package.c:1578)

==10210==    by 0x11CF31: install_packages (clib-package.c:374)

==10210==    by 0x1209B0: clib_package_install_dependencies (clib-package.c:1614)

==10210==    by 0x120863: clib_package_install (clib-package.c:1578)

==10210==    by 0x11B724: main (package-install.c:84)

### Leak summary before fix:
==10210==    definitely lost: 102 bytes in 14 blocks

==10210==    indirectly lost: 0 bytes in 0 blocks

==10210==      possibly lost: 49,591,680 bytes in 337,788 blocks

==10210==    still reachable: 41,801 bytes in 12 blocks

==10210==         suppressed: 0 bytes in 0 blocks

### Leak summary after fix:
==4214==    definitely lost: 0 bytes in 0 blocks

==4214==    indirectly lost: 0 bytes in 0 blocks

==4214==      possibly lost: 49,591,680 bytes in 337,788 blocks

==4214==    still reachable: 41,801 bytes in 12 blocks

==4214==         suppressed: 0 bytes in 0 blocks
