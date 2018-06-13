#!/bin/sh
awk '/VERSION/ {
split ($3, a,.);
a[2] = a[2] + 1;
printf ("#define VERSION %d.%d.%d", a[0], a[1], a[2]);
}' qt/version.h
