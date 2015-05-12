//files.c
#include "files.h"

//file directory
typedef struct _dirList {
    char path[MAX_PATH];
    volatile char nameBuf[DIR_LIST_NAME_BUF_SIZE];
    u32 num;
} dirList_t;

static dirList_t sampleList;

//static function declarations
//memory allocation
static void init_sample_paths(void);
static prgmSampleptr alloc_sample(void);
static void init_sample_param(prgmSample *s, u8 num);

//file handling
static void fake_fread(volatile u8 *dst, u32 len, void *fp);
static char *sample_filepath(s32 n);
static bool strip_extension(char *str);


//extern function definitions
extern void samples_init(void) {
    u8 i;
    
    //  init variables
    n_samples = 0;
    bfinMemCheck = 0;
    bfinSampleSize = 0;
    idx8 = 0;
    idx32 = 0;
    
    //  init array of sample data
    for (i=0; i<N_OFFSETS; i++)
    {
        sample[i] = alloc_sample();
        init_sample_param(sample[i], i);
    }
    print_dbg("\r\n finished alloc_sample() and init_sample_param() ");

    //  setup aux buffers
    sample[0]->offset = 0;
    sample[0]->loop = AUX_SIZE;
    n_samples++;
    bfinMemCheck += AUX_SIZE;
    
    sample[1]->offset = sample[0]->offset + sample[0]->loop;
    sample[1]->loop = AUX_SIZE;
    n_samples++;
    bfinMemCheck += AUX_SIZE;
    
    sample[2]->offset = sample[1]->offset + sample[1]->loop;
    sample[2]->loop = AUX_SIZE;
    n_samples++;
    bfinMemCheck += AUX_SIZE;
    
    sample[3]->offset = sample[2]->offset + sample[2]->loop;
    sample[3]->loop = AUX_SIZE;
    n_samples++;
    bfinMemCheck += AUX_SIZE;
    
    sample[4]->offset = sample[3]->offset + sample[3]->loop;
    sample[4]->loop = AUX_SIZE;
    n_samples++;
    bfinMemCheck += AUX_SIZE;
    
    sample[5]->offset = sample[4]->offset + sample[4]->loop;
    sample[5]->loop = AUX_SIZE;
    n_samples++;
    bfinMemCheck += AUX_SIZE;

    sample[6]->offset = sample[5]->offset + sample[5]->loop;
    sample[6]->loop = AUX_SIZE;
    n_samples++;
    bfinMemCheck += AUX_SIZE;

    sample[7]->offset = sample[6]->offset + sample[6]->loop;
    sample[7]->loop = AUX_SIZE;
    n_samples++;
    bfinMemCheck += AUX_SIZE;
    
    //  off|default sample position
    sample_name[0] = (char*)alloc_mem(MAX_NAME * (sizeof(char*)));
    strcpy(sample_name[0], "-");
    sample[8]->offset = sample[7]->offset + sample[7]->loop;
    sample[8]->loop = 1024;
    n_samples++;
    bfinMemCheck += 1024;

    //  samples from card from here!
    sample[9]->offset = sample[8]->offset + sample[8]->loop;

    //  init wav file paths
    init_sample_paths();
    print_dbg("\r\n finished init_sample_paths() ");
}

prgmSampleptr alloc_sample(void) {
    return(prgmSampleptr)alloc_mem(sizeof(prgmSample));
}

void init_sample_param(prgmSample *s, u8 num) {
    s->num = num;
    s->offset = 0;
    s->loop = 0;
    s->offset_cut = 0;
    s->loop_cut = -1;
}

void init_sample_paths(void) {
    FL_DIR dirstat;
    struct fs_dir_ent dirent; //see fat_access.h
    
    s32 i = 0;
    sampleList.num = 0;
        
    strcpy(sampleList.path, WAV_PATH);
    
    if (fl_opendir(sampleList.path, &dirstat))
    {
        //  check samples limit
        while (fl_readdir(&dirstat, &dirent) == 0 && n_samples < N_OFFSETS)
        {
            if (!(dirent.is_dir))
            {
                sample_path[i] = (char*)alloc_mem(MAX_PATH * (sizeof(char*)));
                sample_name[i+1] = (char*)alloc_mem(MAX_NAME * (sizeof(char*)));
                strcpy(sampleList.path, WAV_PATH);
                strcat(sampleList.path, dirent.filename);
                
                //  copy name for screen display and remove extension
                strcpy(sample_name[i+1], dirent.filename);
                strip_extension(sample_name[i+1]);
                
                //  copy file path
                strcpy(sample_path[i], sampleList.path);
                print_dbg("\r\n sample_path[i]: ");
                print_dbg(sample_path[i]);
                sampleList.num++;

                i++;
                print_dbg("\r\n i: ");
                print_dbg_ulong(i);
                
                n_samples++;
                print_dbg("\r\n n_samples: ");
                print_dbg_ulong(n_samples);
            }
        }
        //fl_closedir(&dirstat);
    }
}

char *sample_filepath(s32 n) {
    return (n < 0 || n > n_samples) ? sample_path[0] : sample_path[n];
}

void files_load_sample(u8 n) {
    //  pointer to file, size
    void *fp;
    u32 size = 0;
    idx32 = 0;
    bfinSampleSize = 0;

    delay_ms(10);
    
    fp = fl_fopen(sample_filepath(n-9), "r");
    print_dbg("\r\n sample[n]->num ");
    print_dbg_ulong(sample[n]->num);
    print_dbg("\r\n sample_filepath(n) ");
    print_dbg(sample_filepath(n-9));
    
    size = ((FL_FILE*)(fp))->filelength;
    sample[n]->loop = size / sizeof(s32);
    sample[n+1]->offset = sample[n]->offset + sample[n]->loop;
    
    if (fp != NULL)
    {
        //  allocate a temporary RAM buffer
        bfinSampleData = (u8*)alloc_mem(size);
        fake_fread(bfinSampleData, size, fp);
        fl_fclose(fp);
        
        bfinMemCheck += sample[smpl]->loop;
        
        //  check bfin sdram buffer size limit
        if (bfinMemCheck < BFIN_BUFFER_SIZE)
        {
            //  sample transfer happens in custom_timer_callback()
            idx8 = 0;
            bfinSampleSize = size;
        }
        else
        {
            bfinSampleSize = 0;
            smpl = N_OFFSETS + 1;
        }
    }
    else print_dbg("\r\n file contains no data");
}

u8 files_load_dsp(void) {
    void *fp;
    u32 size = 0;
    u8 ret;
    
    delay_ms(10);
    
    app_pause();
    
    fp = fl_fopen(DSP_PATH, "r");
    
    if(fp != NULL) {
        size = ((FL_FILE*)(fp))->filelength;
        fake_fread(bfinLdrData, size, fp);
        
        fl_fclose(fp);
        bfinLdrSize = size;
        
        if(bfinLdrSize > 0) {
            bfin_load_buf(bfinLdrData, bfinLdrSize);
            ret = 1;
        } else {
            print_dbg("\r\n bfin ldr size was <=0, aborting");
            ret = 0;
        }
    } else {
        print_dbg("\r\n error: fp was null in files_load_dsp_name \r\n");
        ret = 0;
    }
    
    app_resume();
    return ret;
}

void fake_fread(volatile u8 *dst, u32 len, void *fp) { //fread: no size arg
    u32 n = 0;
#if 0
    fl_fread(&dst, 1, len, fp);
#else
    while(n < len) {
        *dst = fl_fgetc(fp);
        n++;
        dst++;
    }
#endif
}

bool strip_extension(char *str) {
    int i;
    int dotpos = -1;
    i = strlen(str);
    while(i > 0) {
        --i;
        if(str[i] == '.') {
            dotpos = i;
            break;
        }
    }
    if(dotpos >= 0) {
        str[dotpos] = '\0';
        return 1;
    } else {
        return 0;
    }
}
