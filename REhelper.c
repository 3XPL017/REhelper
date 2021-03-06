#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <sys/ptrace.h>
#include <sys/utsname.h>

#define MAX 0xFF

void ascii_table(){
    char output[MAX];

    for(int i = 0x20; i <= 0x7f; i++){
        output[0] = i;
        printf("Hex value:   %x\n", i);
        printf("Ascii value: %c\n", output[0]);
        printf("\n");
    }
}

void reverse(char *string){
    int length = strnlen(string, MAX) - 1;
    char output[MAX];

    for(int i = 0; i <= length; i++){
        output[i] = string[length - i];
    }

    printf("%s\n", output);
}

void pattern(char *string, int value){
    for(int i = 0; i < value; i++){
        printf("%s", string);
    }

    printf("\n");
}

void magic(char *string){
    char *filecontent;
    FILE *file;

    file = fopen(string, "rb");

    if(!file){
        printf("File not found\n");
        return;
    }

    filecontent = malloc(sizeof(file));
    fscanf(file, "%s", filecontent);

    if(filecontent[0] != 0x7f){
        printf("Not an ELF file\n");
        return;
    }else if(filecontent[1] != 0x45){
        printf("Not an ELF file\n");
        return;
    }else if(filecontent[2] != 0x4c){
        printf("Not an ELF file\n");
        return;
    }else if(filecontent[3] != 0x46){
        printf("Not an ELF file\n");
        return;
    }

    printf("ELF magic: ");

    for(int i = 0; i <= 6; i++){
        printf("%02x ", filecontent[i]);
    }

    printf("\n");
    free(filecontent);
    filecontent = NULL;
}

void dump_stack(char *string){
    char *filecontent;
    char stack[MAX] = "/proc/";
    FILE *file;

    strncat(stack, string, MAX - strnlen(stack, MAX));
    strncat(stack, "/stack", MAX - strnlen(stack, MAX));
    file = fopen(stack, "r");

    if(!file){
        printf("No such process\n");
        return;
    }

    filecontent = malloc(sizeof(file));
    fread(filecontent, sizeof(file), 45, file);
    printf("%s", filecontent);
    free(filecontent);
    filecontent = NULL;
}

void dump_status(char *string){
    char *filecontent;
    char stack[MAX] = "/proc/";
    FILE *file;

    strncat(stack, string, MAX - strnlen(stack, MAX));
    strncat(stack, "/status", MAX - strnlen(stack, MAX));
    file = fopen(stack, "r");

    if(!file){
        printf("No such process\n");
        return;
    }

    filecontent = malloc(sizeof(file));
    fread(filecontent, sizeof(file), 20, file);
    printf("%s", filecontent);
    free(filecontent);
    filecontent = NULL;
}

void dump_io(char *string){
    char *filecontent;
    char stack[MAX] = "/proc/";
    FILE *file;

    strncat(stack, string, MAX - strnlen(stack, MAX));
    strncat(stack, "/io", MAX - strnlen(stack, MAX));
    file = fopen(stack, "r");

    if(!file){
        printf("No such process\n");
        return;
    }

    filecontent = malloc(sizeof(file));
    fread(filecontent, sizeof(file), 8, file);
    printf("%s", filecontent);
    free(filecontent);
    filecontent = NULL;
}

void dump_maps(char *string){
    char *filecontent;
    char stack[MAX] = "/proc/";
    FILE *file;

    strncat(stack, string, MAX - strnlen(stack, MAX));
    strncat(stack, "/maps", MAX - strnlen(stack, MAX));
    file = fopen(stack, "r");

    if(!file){
        printf("No such process\n");
        return;
    }

    filecontent = malloc(sizeof(file));
    fread(filecontent, sizeof(file), 400, file);
    printf("%s", filecontent);
    printf("\n");
    free(filecontent);
    filecontent = NULL;
}

void dump_limits(char *string){
    char *filecontent;
    char stack[MAX] = "/proc/";
    FILE *file;

    strncat(stack, string, MAX - strnlen(stack, MAX));
    strncat(stack, "/limits", MAX - strnlen(stack, MAX));
    file = fopen(stack, "r");

    if(!file){
        printf("No such process\n");
        return;
    }

    filecontent = malloc(sizeof(file));
    fread(filecontent, sizeof(file), 128, file);
    printf("%s", filecontent);
    printf("\n");
    free(filecontent);
    filecontent = NULL;
}

void dump_clone(char *string){
    int outfile;
    char *filecontent;
    char clone[MAX] = "/proc/";
    FILE *file;
    FILE *cloned;
    FILE *out;

    strncat(clone, string, MAX - strnlen(clone, MAX));
    strncat(clone, "/exe", MAX - strnlen(clone, MAX));
    file = fopen(clone, "r");
    cloned = fopen("cloned", "r");
    out = fopen("cloned", "w");

    if(!file){
        printf("No such process\n");
        return;
    }

    if(cloned){
        printf("File 'cloned' already exists\n");
        return;
    }

    if(!out){
        printf("Cannot write to file\n");
        return;
    }

    outfile = fgetc(file);

    while(outfile != EOF){
        fputc(outfile, out);
        outfile = fgetc(file);
    }

    system("chmod +x cloned");
    printf("Created file named cloned\n");
    
}

void aslr(){
    system("echo 2 | sudo tee /proc/sys/kernel/randomize_va_space");
}

void no_aslr(){
    system("echo 0 | sudo tee /proc/sys/kernel/randomize_va_space");
}

void sig_handler(int signo){
    exit(0);
}

void no_time(){
    signal(SIGINT, sig_handler);

    for(;;){
        system("date -s '06 JUL 2018 12:00:00'");
        usleep(30000);
    }
}

void version(){
    struct utsname versionData;

    uname(&versionData);
    printf("System:  %s\n", versionData.sysname);
    printf("Distro:  %s\n", versionData.nodename);
    printf("Release: %s\n", versionData.release);
    printf("Version: %s\n", versionData.version);
    printf("Machine: %s\n", versionData.machine);
}

void add(char *string){
    int total = 0;
    int length = strnlen(string, MAX) - 1;

    for(int i = 0; i <= length; i++){
        total = total + string[i];
    }

    printf("Decimal value: %d", total);
    printf("\n");
    printf("Hex value:     %x", total);
    printf("\n");
}

void sub(char *string, int value){
    int length = strnlen(string, MAX) - 1;

    for(int i = 0; i <= length; i++){
        value = value - string[i];
    }

    printf("Decimal value: %d", value);
    printf("\n");
    printf("Hex value:     %x", value);
    printf("\n");
}

void mult(char *string){
    int total = 1;
    int length = strnlen(string, MAX) - 1;

    for(int i = 0; i <= length; i++){
        total = total * string[i];
    }

    printf("Decimal value: %d", total);
    printf("\n");
    printf("Hex value:     %x", total);
    printf("\n");
}

void shift(char *string, int shift){
    int length = strnlen(string, MAX) - 1;
    char rightshifted[MAX];
    char leftshifted[MAX];

    leftshifted[length + 1] = '\0';
    rightshifted[length + 1] = '\0';
    printf("Shifted right: ");

    for(int i = 0; i <= length; i++){
        rightshifted[i] = string[i] >> shift;
        printf("%x", rightshifted[i]);
    }

    printf("\n");
    printf("Shifted left:  ");

    for(int e = 0; e <= length; e++){
        leftshifted[e] = string[e] << shift;
        printf("%x", leftshifted[e]);
    }

    printf("\n");
}

void xor(char *string, int xor){
    int length = strnlen(string, MAX) - 1;
    char xored[MAX];

    printf("Hex value: ");

    for(int i = 0; i <= length; i++){
        xored[i] = string[i] ^ xor;
        printf("%x", xored[i]);
    }

    xored[length + 1] = '\0';
    printf("\n");
    printf("String:    ");
    printf("%s\n", xored);
}

void xors(char *string){
    int xor;
    int length = strnlen(string, MAX) - 1;
    char xored[length];

    xored[length + 1] = '\0';

    for(int i = 0; i <= 256; i++){
        printf("Hex value %d: ", i);

        for(int e = 0; e <= length; e++){
            xor = string[e] ^ i;
            xored[e] = (char)(xor);
            printf("%x", xored[e]);
        }

        printf("\n");
        printf("String    %d: ", i);
        printf("%s\n", xored);
    }
}

void base64encode(char *string){
    BIO *base64;
    BIO *input;

    base64 = BIO_new(BIO_f_base64());
    input = BIO_new_fp(stdout, BIO_NOCLOSE);
    BIO_push(base64, input);
    BIO_write(base64, string, strnlen(string, MAX));
    BIO_flush(base64);
    BIO_free_all(base64);
    base64 = NULL;
}

int length_decode(char *string){ 
    int equals;
    int length = strnlen(string, MAX);

    if(string[length - 1] == '=' && string[length - 2] == '='){
        equals = 2;
    }else if(string[length - 1] == '='){
        equals = 1;
    }else{
        equals = 0;
    }

    return (length * 3) / 4 - equals;
}

void base64decode(char *string){
    int length;
    int decode_length = length_decode(string);
    char *output;
    BIO *base64;
    BIO *input;

    output = (unsigned char *)malloc(decode_length + 1);
    output[decode_length] = '\0';
    input = BIO_new_mem_buf(string, -1);
    base64 = BIO_new(BIO_f_base64());
    input = BIO_push(base64, input);
    BIO_set_flags(input, BIO_FLAGS_BASE64_NO_NL); 
    length = BIO_read(input, output, strnlen(string, MAX));
    assert(length == decode_length); 
    BIO_free_all(input);
    input = NULL;
    printf("%s\n", output);
}

void getmd5(char *string){
    int length = strnlen(string, MAX);
    unsigned char hash[MD5_DIGEST_LENGTH];
    MD5_CTX ctx;

    MD5_Init(&ctx);
    MD5_Update(&ctx, string, length);
    MD5_Final(hash, &ctx);
    printf("\n");    

    for(int i = 0; i < 16; i++){
        printf("%02x", (unsigned int)hash[i]);
    }

    printf("\n");
}

void getsha1(char *string){
    int length = strnlen(string, MAX);
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA_CTX ctx;

    SHA1_Init(&ctx);
    SHA1_Update(&ctx, string, length);
    SHA1_Final(hash, &ctx);
    printf("\n");    

    for(int i = 0; i < 20; i++){
        printf("%02x", (unsigned int)hash[i]);
    }

    printf("\n");    
}

void getsha256(char *string){
    int length = strnlen(string, MAX);
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX ctx;

    SHA256_Init(&ctx);
    SHA256_Update(&ctx, string, length);
    SHA256_Final(hash, &ctx);
    printf("\n");    

    for(int i = 0; i < 32; i++){
        printf("%02x", (unsigned int)hash[i]);
    }

    printf("\n");
}

void getsha384(char *string){
    int length = strnlen(string, MAX);
    unsigned char hash[SHA384_DIGEST_LENGTH];
    SHA512_CTX ctx;

    SHA384_Init(&ctx);
    SHA384_Update(&ctx, string, length);
    SHA384_Final(hash, &ctx);
    printf("\n");    

    for(int i = 0; i < 48; i++){
        printf("%02x", (unsigned int)hash[i]);
    }

    printf("\n");
}

void getsha512(char *string){
    int length = strnlen(string, MAX);
    unsigned char hash[SHA512_DIGEST_LENGTH];
    SHA512_CTX ctx;

    SHA512_Init(&ctx);
    SHA512_Update(&ctx, string, length);
    SHA512_Final(hash, &ctx);
    printf("\n");    

    for(int i = 0; i < 64; i++){
        printf("%02x", (unsigned int)hash[i]);
    }

    printf("\n");
}

void trace(){
    if(ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1) {
        printf("Debugger detected!\n");
        exit(0);
    }else{
        printf("No debugger detected\n");
        return;
    }
}

void detect(){
    if(ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1) {
        printf("Debugger detected!\n");
        exit(0);
    }else{
        return;
    }
}

void breakpoint(){
    printf("Breakpoint hit at: %p\n", breakpoint);
    __asm__("int $3");
}

void background(){
    detect();
    __asm__("int $3");
}

void crash(){
    __asm__("push $0xff");
}

void address(){
    void pick_one();
    void pick_two();
    void pick_three();
    void usage();
    int main();

    printf("ascii:   %p\n", ascii_table);
    printf("reverse: %p\n", reverse);
    printf("pattern: %p\n", pattern);
    printf("magic:   %p\n", magic);
    printf("stack:   %p\n", dump_stack);
    printf("status:  %p\n", dump_status);
    printf("io:      %p\n", dump_io);
    printf("maps:    %p\n", dump_maps);
    printf("limits:  %p\n", dump_limits);
    printf("clone:   %p\n", dump_clone);
    printf("aslr:    %p\n", aslr);
    printf("no-aslr: %p\n", no_aslr);
    printf("no-time: %p\n", no_time);
    printf("version: %p\n", version);
    printf("add:     %p\n", add);
    printf("sub:     %p\n", sub);
    printf("mult:    %p\n", mult);
    printf("shift:   %p\n", shift);
    printf("xor:     %p\n", xor);
    printf("xors:    %p\n", xors);
    printf("base64e  %p\n", base64encode);
    printf("b64len   %p\n", length_decode);
    printf("base64d  %p\n", base64decode);
    printf("md5:     %p\n", getmd5);
    printf("sha1:    %p\n", getsha1);
    printf("sha256:  %p\n", getsha256);
    printf("sha384:  %p\n", getsha384);
    printf("sha512:  %p\n", getsha512);
    printf("ptrace:  %p\n", trace);
    printf("ptrace1: %p\n", detect);
    printf("break:   %p\n", breakpoint);
    printf("back:    %p\n", background);
    printf("crash:   %p\n", crash);
    printf("address: %p\n", address);
    printf("pick1:   %p\n", pick_one);
    printf("pick2:   %p\n", pick_two);
    printf("pick3:   %p\n", pick_three);
    printf("usage:   %p\n", usage);
    printf("main:    %p\n", main);
}

void pick_one(char *option){
    if(strncmp(option, "--ascii", MAX) == 0){
        ascii_table();
        exit(0);
    }else if(strncmp(option, "--aslr", MAX) == 0){
        aslr();
        exit(0);
    }else if(strncmp(option, "--no-aslr", MAX) == 0){
        no_aslr();
        exit(0);
    }else if(strncmp(option, "--no-time", MAX) == 0){
        no_time();
        exit(0);
    }else if(strncmp(option, "--version", MAX) == 0){
        version();
        exit(0);
    }else if(strncmp(option, "--ptrace", MAX) == 0){
        trace();
        exit(0);
    }else if(strncmp(option, "--break", MAX) == 0){
        breakpoint();
        exit(0);
    }else if(strncmp(option, "--back", MAX) == 0){
        background();
        exit(0);
    }else if(strncmp(option, "--crash", MAX) == 0){
        crash();
        exit(0);
    }else if(strncmp(option, "--address", MAX) == 0){
        address();
        exit(0);
    }else{
        return;
    }
}

void pick_two(char *option, char *string){
    if(strncmp(option, "--reverse", MAX) == 0){
        reverse(string);
        exit(0);
    }else if(strncmp(option, "--magic", MAX) == 0){
        magic(string);
        exit(0);
    }else if(strncmp(option, "--stack", MAX) == 0){
        dump_stack(string);
        exit(0);
    }else if(strncmp(option, "--status", MAX) == 0){
        dump_status(string);
        exit(0);
    }else if(strncmp(option, "--io", MAX) == 0){
        dump_io(string);
        exit(0);
    }else if(strncmp(option, "--maps", MAX) == 0){
        dump_maps(string);
        exit(0);
    }else if(strncmp(option, "--limits", MAX) == 0){
        dump_limits(string);
        exit(0);
    }else if(strncmp(option, "--clone", MAX) == 0){
        dump_clone(string);
        exit(0);
    }else if(strncmp(option, "--add", MAX) == 0){
        add(string);
        exit(0);
    }else if(strncmp(option, "--mult", MAX) == 0){
        mult(string);
        exit(0);
    }else if(strncmp(option, "--xors", MAX) == 0){
        xors(string);
        exit(0);
    }else if(strncmp(option, "--base64e", MAX) == 0){
        base64encode(string);
        exit(0);
    }else if(strncmp(option, "--base64d", MAX) == 0){
        base64decode(string);
        exit(0);
    }else if(strncmp(option, "--md5", MAX) == 0){
        getmd5(string);
        exit(0);
    }else if(strncmp(option, "--sha1", MAX) == 0){
        getsha1(string);
        exit(0);
    }else if(strncmp(option, "--sha256", MAX) == 0){
        getsha256(string);
        exit(0);
    }else if(strncmp(option, "--sha384", MAX) == 0){
        getsha384(string);
        exit(0);
    }else if(strncmp(option, "--sha512", MAX) == 0){
        getsha512(string);
        exit(0);
    }else{
        return;
    }
}

void pick_three(char *option, char *string, int value){
    if(strncmp(option, "--pattern", MAX) == 0){
        pattern(string, value);
        exit(0);
    }else if(strncmp(option, "--sub", MAX) == 0){
        sub(string, value);
        exit(0);
    }else if(strncmp(option, "--shift", MAX) == 0){
        shift(string, value);
        exit(0);
    }else if(strncmp(option, "--xor", MAX) == 0){\
        xor(string, value);
        exit(0);
    }else{
        return;
    }
}

void usage(){
    printf("REhelper created by Andromeda\n");
    printf("Usage <option> <string> <number>\n\n");
    printf("General options:\n");
    printf("--ascii                         prints ascii table\n");
    printf("--reverse   <string>            reverses the string\n");
    printf("--pattern   <string> <number>   prints string the given ammount of times\n\n");
    printf("Binary options:\n");
    printf("--magic     <file>              prints magic bytes from ELF files\n\n");
    printf("Process options:\n");
    printf("--stack     <string>            give process pid to dump its stack\n");
    printf("--status    <string>            give process pid to dump its status\n");
    printf("--io        <string>            give process pid to dump its IO data\n");
    printf("--maps      <string>            give process pid to dump its mappings\n");
    printf("--limits    <string>            give process pid to dump its limits\n");
    printf("--clone     <string>            give process pid to dump its binary\n\n");
    printf("System options:\n");
    printf("--aslr                          turns on system ASLR\n");
    printf("--no-aslr                       turns off system ASLR\n");
    printf("--no-time                       makes time stop\n");
    printf("--version                       prints system information\n\n");
    printf("Logic options:\n");
    printf("--add       <string>            adds value of string together\n");
    printf("--sub       <string> <number>   subtracts value of string from a value\n");
    printf("--mult      <string>            multiplies value of string together\n");
    printf("--shift     <string> <number>   bit shifts a string by a value\n");
    printf("--xor       <string> <number>   xores a string by a value\n");
    printf("--xors      <string>            xores a string by all 256 values\n\n");
    printf("Encoding options:\n");
    printf("--base64e   <string>            encodes a string in base64\n");
    printf("--base64d   <string>            decodes a base64 string\n\n");
    printf("Hashing options:\n");
    printf("--md5       <string>            prints md5 of a string\n");
    printf("--sha1      <string>            prints sha1 of a string\n");
    printf("--sha256    <string>            prints sha256 of a string\n");
    printf("--sha384    <string>            prints sha384 of a string\n");
    printf("--sha512    <string>            prints sha512 of a string\n\n");
    printf("Debug options:\n");
    printf("--ptrace                        calls ptrace\n");
    printf("--break                         calls int3\n");
    printf("--back                          continue execution in background\n");
    printf("--crash                         cause segmentation fault\n");
    printf("--address                       shows memory address of all functions\n\n");
}

int main(int argc, char **argv){
    int value;

    if(argc == 2){
        pick_one(argv[1]);
    }else if(argc == 3){
        pick_two(argv[1], argv[2]);
    }else if(argc == 4){
        value = atoi(argv[3]);
        pick_three(argv[1], argv[2], value);
    }
        
    usage();
    return 0;
    
}