int test(int);


int __start() {
    int a = 0xFFF;
    int dd = 0xDD;
    a = test(a+dd);
    return a;
}

int test(int i) {
    int f = 0x11;
    return i+f;
}



