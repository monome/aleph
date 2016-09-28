//files.c
#include "files.h"

//file directory
typedef struct _dirList {
    char path[MAX_PATH];
    volatile char nameBuf[DIR_LIST_NAME_BUF_SIZE];
    u32 num;
} dirList_t;

static dirList_t fileList;

volatile u8 *prgmFileData;
volatile u32 prgmFileSize = 0;


static const int eParamSetTrigId[] =
{
    eParamSetTrig0,
    eParamSetTrig1,
    eParamSetTrig2,
    eParamSetTrig3,
    eParamSetTrig4,
    eParamSetTrig5,
    eParamSetTrig6,
    eParamSetTrig7,
};

static const int eParamSampleOffsetAId[] =
{
    eParamSampleOffsetA0,
    eParamSampleOffsetA1,
    eParamSampleOffsetA2,
    eParamSampleOffsetA3,
    eParamSampleOffsetA4,
    eParamSampleOffsetA5,
    eParamSampleOffsetA6,
    eParamSampleOffsetA7,
};

static const int eParamSampleOffsetBId[] =
{
    eParamSampleOffsetB0,
    eParamSampleOffsetB1,
    eParamSampleOffsetB2,
    eParamSampleOffsetB3,
    eParamSampleOffsetB4,
    eParamSampleOffsetB5,
    eParamSampleOffsetB6,
    eParamSampleOffsetB7,
};

static const int eParamOutputLevelId[] =
{
    eParamOutputLevel0,
    eParamOutputLevel1,
    eParamOutputLevel2,
    eParamOutputLevel3,
    eParamOutputLevel4,
    eParamOutputLevel5,
    eParamOutputLevel6,
    eParamOutputLevel7,
};


//static function declarations
//memory allocation
static void init_sample_paths(void);
static prgmSampleptr alloc_sample(void);
static void init_sample_param(prgmSample *s, u8 num);
static void init_pattern_paths(void);
static void init_composition_paths(void);
static void init_scene_paths(void);

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
    sample[0]->offset = 0x00000000;
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
    fileList.num = 0;
        
    strcpy(fileList.path, WAV_PATH);
    
    if (fl_opendir(fileList.path, &dirstat))
    {
        //  check samples limit
        while (fl_readdir(&dirstat, &dirent) == 0 && n_samples < N_OFFSETS)
        {
            if (!(dirent.is_dir))
            {
                sample_path[i] = (char*)alloc_mem(MAX_PATH * (sizeof(char*)));
                sample_name[i+1] = (char*)alloc_mem(MAX_NAME * (sizeof(char*)));
                strcpy(fileList.path, WAV_PATH);
                strcat(fileList.path, dirent.filename);
                
                //  copy name for screen display and remove extension
                strcpy(sample_name[i+1], dirent.filename);
                strip_extension(sample_name[i+1]);
                
                //  copy file path
                strcpy(sample_path[i], fileList.path);
                print_dbg("\r\n sample_path[i]: ");
                print_dbg(sample_path[i]);
                fileList.num++;

                i++;
                print_dbg("\r\n i: ");
                print_dbg_ulong(i);
                
                n_samples++;
                print_dbg("\r\n n_samples: ");
                print_dbg_ulong(n_samples);
            }
        }
//        fl_closedir(&dirstat);
    }
}

char *sample_filepath(s32 n) {
    return (n < 0 || n > n_samples) ? sample_path[0] : sample_path[n];
}

void deck_init(void) {
    int i, t, s;
    
    //  send software reset to stm32
    ctl_command_change(10, 0, 0, 0, 0, 0, 0, 0);
    
    //  wait for stm32 reset process
    delay_ms(300);
    
    //  load and transfer patterns
    for (i=n_pdx; i>0; i--)
    {
        //load file to avr32
        npdx = i - 1;
        render_boot(pdx_name[npdx]);
        files_load_npdx(pdx_path, npdx);
//        print_dbg("\r\n npdx: ");
//        print_dbg_ulong(npdx);
        
        //  transfer pattern to STM32 (i2c)
        for (t=0; t<N_TRK; t++)
        {
            for (s=0; s<SQ_LEN; s++)
            {
                u8 cmd = trk_deck[t]->s[s];
                set_command(npdx, t, s, cmd);
                
                //  wait for stm32 pattern write
                delay_ms(3);
            }
            ctl_command_change(4, npdx, t, trk_deck[t]->len, 0, 0, 0, 0);
            
            //  wait for stm32 pattern write
            delay_ms(3);
        }
    }
    
    //  enable external clock to stm32 | performance mode (default)
    clock_mode = PERFORMANCE;
    
    //  return to 1
    ctl_command_change(1, 0, 0, 0, 0, 0, 0, 0);
    ctl_param_change(DUMMY, eParamResetCounter, DUMMY);
}

void patterns_init(void) {
    init_pattern_paths();
    
    dirty_pdx = 0;
    dirty_prgm = 0;
}

void init_pattern_paths(void) {
    FL_DIR dirstat;
    static struct fs_dir_ent dirent;
    u16 i;
    
    //  prgm patterns
    i = 0;
    fileList.num = 0;
    n_prgm = 0;
    
    strcpy(fileList.path, PRGM_PATH);
    
    if (fl_opendir(fileList.path, &dirstat))
    {
        while (fl_readdir(&dirstat, &dirent) == 0)
        {
            if(!(dirent.is_dir))
            {
                prgm_path[i] = (char*)alloc_mem(MAX_PATH * (sizeof(char*)));
                prgm_name[i] = (char*)alloc_mem(MAX_NAME * (sizeof(char*)));
                strcpy(fileList.path, PRGM_PATH);
                strcat(fileList.path, dirent.filename);
                
                //  copy name for screen display and remove extension
                strcpy(prgm_name[i], dirent.filename);
                strip_extension(prgm_name[i]);
                
                //  copy file path
                strcpy(prgm_path[i], fileList.path);
                fileList.num++;
                i++;
                n_prgm++;
            }
        }
        fl_closedir(&dirstat);
    }

    //  pdx patterns
    i = 0;
    fileList.num = 0;
    n_pdx = 0;

    strcpy(fileList.path, PDX_PATH);
    
    if (fl_opendir(fileList.path, &dirstat))
    {
        while (fl_readdir(&dirstat, &dirent) == 0)
        {
            if(!(dirent.is_dir))
            {
                pdx_path[i] = (char*)alloc_mem(MAX_PATH * (sizeof(char*)));
                pdx_name[i] = (char*)alloc_mem(MAX_NAME * (sizeof(char*)));
                strcpy(fileList.path, PDX_PATH);
                strcat(fileList.path, dirent.filename);
                
                //  copy name for screen display and remove extension
                strcpy(pdx_name[i], dirent.filename);
                strip_extension(pdx_name[i]);

                //  copy file path
                strcpy(pdx_path[i], fileList.path);
                fileList.num++;
                i++;
                n_pdx++;
            }
        }
        fl_closedir(&dirstat);
    }
}

void composition_init(void) {
    init_composition_paths();
    
    dirty_cps = 0;
}

void init_composition_paths(void) {
    FL_DIR dirstat;
    static struct fs_dir_ent dirent;
    u16 i;
    
    //  prgm patterns
    i = 0;
    fileList.num = 0;
    n_cps = 0;
    
    strcpy(fileList.path, CPS_PATH);
    
    if (fl_opendir(fileList.path, &dirstat))
    {
        while (fl_readdir(&dirstat, &dirent) == 0)
        {
            if(!(dirent.is_dir))
            {
                cps_path[i] = (char*)alloc_mem(MAX_PATH * (sizeof(char*)));
                cps_name[i] = (char*)alloc_mem(MAX_NAME * (sizeof(char*)));
                strcpy(fileList.path, CPS_PATH);
                strcat(fileList.path, dirent.filename);
                
                //  copy name for screen display and remove extension
                strcpy(cps_name[i], dirent.filename);
                strip_extension(cps_name[i]);
                
                //  copy file path
                strcpy(cps_path[i], fileList.path);
                fileList.num++;
                i++;
                n_cps++;
            }
        }
        fl_closedir(&dirstat);
    }
}

void scene_init(void) {
    init_scene_paths();
    
    dirty_scn = 0;
}

void init_scene_paths(void) {
    FL_DIR dirstat;
    static struct fs_dir_ent dirent;
    u16 i;
    
    //  prgm patterns
    i = 0;
    fileList.num = 0;
    n_scn = 0;
    
    strcpy(fileList.path, SCN_PATH);
    
    if (fl_opendir(fileList.path, &dirstat))
    {
        while (fl_readdir(&dirstat, &dirent) == 0)
        {
            if(!(dirent.is_dir))
            {
                scn_path[i] = (char*)alloc_mem(MAX_PATH * (sizeof(char*)));
                scn_name[i] = (char*)alloc_mem(MAX_NAME * (sizeof(char*)));
                strcpy(fileList.path, SCN_PATH);
                strcat(fileList.path, dirent.filename);
                
                //  copy name for screen display and remove extension
                strcpy(scn_name[i], dirent.filename);
                strip_extension(scn_name[i]);
                
                //  copy file path
                strcpy(scn_path[i], fileList.path);
                fileList.num++;
                i++;
                n_scn++;
            }
        }
        fl_closedir(&dirstat);
    }
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
        
        //TRY USE A CUSTOM EVENT TO TRANSFER 1 SAMPLE AT A TIME
        
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

uint8_t files_load_nprgm(char *p[], u8 n) {
//    FL_DIR dirstat;
//    struct fs_dir_ent dirent;
    void *fp;
    
    ParamValueSwap tmp;
    u32 bytes = 0;
    uint8_t t, s;

    app_pause();
    
    /*
    //  open directory
    if(fl_opendir(PRGM_PATH, &dirstat))
    {
        while (fl_readdir(&dirstat, &dirent) == 0)
        {
            print_dbg("\r\n fs_dir_list_status sector: ");
            print_dbg_ulong(dirstat.sector);
            print_dbg("\r\n fs_dir_list_status cluster: ");
            print_dbg_ulong(dirstat.cluster);
            print_dbg("\r\n fs_dir_list_status offset: ");
            print_dbg_ulong(dirstat.offset);
            
            print_dbg("\r\n fs_dir_ent filename: ");
            print_dbg(dirent.filename);
            print_dbg("\r\n fs_dir_ent is_dir: ");
            print_dbg_ulong(dirent.is_dir);
            print_dbg("\r\n fs_dir_ent cluster: ");
            print_dbg_ulong(dirent.cluster);
            print_dbg("\r\n fs_dir_ent size: ");
            print_dbg_ulong(dirent.size);

            char name[MAX_NAME];
            strcpy(name, dirent.filename);
            strip_extension(name);
            if (strcmp(name, prgm_name[n]) == 0)
            {
                print_dbg("\r\n found match...");
                print_dbg(name);
                fp = fl_fopen(p[n], "r");
                prgmFileSize = dirent.size;
                break;
            }
            else
            {
                prgmFileSize = 0;
                fp = NULL;
            }
        }
    }
    else
    {
        print_dbg("\r\n dir error...");
        prgmFileSize = 0;
        fp = NULL;
    }
    */

    //  load from file
    fp = fl_fopen(p[n], "r");
    prgmFileSize = N_TRK * sizeof(prgmTrack);

    prgmFileData = (u8*)alloc_mem(FILE_SIZE);
    fake_fread(prgmFileData, prgmFileSize, fp);
    fl_fclose(fp);
//    fl_closedir(&dirstat);
    
    //  copy data
    for (t=0; t<N_TRK; t++)
    {
        for (s=0; s<SQ_LEN; s++)
        {
            trk[t]->s[s] = prgmFileData[bytes];
            bytes++;
            
            tmp.asByte[0] = prgmFileData[bytes];
            bytes++;
            tmp.asByte[1] = prgmFileData[bytes];
            bytes++;
            tmp.asByte[2] = prgmFileData[bytes];
            bytes++;
            tmp.asByte[3] = prgmFileData[bytes];
            bytes++;
            trk[t]->l[s] = tmp.asInt;
        }
        trk[t]->len = prgmFileData[bytes];
        bytes++;
    }
    
    //  transfer pattern to BFIN (spi)
    for (t=0; t<N_TRK; t++)
    {
        for (s=0; s<SQ_LEN; s++)
        {
            //transfer trigs (column 1)
            s8 tmp = trk[t]->s[s];
            if (tmp == N_BUFFERS) ctl_param_change(s, eParamSetTrigId[t], 0);
            if (tmp != N_BUFFERS) ctl_param_change(s, eParamSetTrigId[t], 1);
            
            //transfer offsets (column 1)
            ctl_param_change(s, eParamSampleOffsetBId[t], sample[tmp + 1]->offset);
            ctl_param_change(s, eParamSampleOffsetAId[t], sample[tmp]->offset);
            
            //transfers levels (column 2)
            ctl_param_change(s, eParamOutputLevelId[t], trk[t]->l[s]);
        }
        ctl_param_change(t, eParamTrkLength, trk[t]->len);
    }
    
    free_mem(prgmFileData);

    app_resume();
    
    return 0;
}

uint8_t files_save_nprgm(char *p[], u8 n) {
//    FL_DIR dirstat;
//    struct fs_dir_ent dirent;
    void *fp;
    
    ParamValueSwap tmp;
    u32 bytes = 0;
    uint8_t t, s;
    
    app_pause();
    
    prgmFileData = (u8*)alloc_mem(FILE_SIZE);
    
    //write pattern to buffer
    for (t=0; t<N_TRK; t++)
    {
        for (s=0; s<SQ_LEN; s++)
        {
            prgmFileData[bytes] = trk[t]->s[s];
            bytes++;
                
            tmp.asInt = trk[t]->l[s];
            prgmFileData[bytes] = tmp.asByte[0];
            bytes++;
            prgmFileData[bytes] = tmp.asByte[1];
            bytes++;
            prgmFileData[bytes] = tmp.asByte[2];
            bytes++;
            prgmFileData[bytes] = tmp.asByte[3];
            bytes++;
        }
        prgmFileData[bytes] = trk[t]->len;
        bytes++;
    }
    
    /*
    //  open directory
    if(fl_opendir(PRGM_PATH, &dirstat))
    {
        while (fl_readdir(&dirstat, &dirent) == 0)
        {
            char name[MAX_NAME];
            strcpy(name, dirent.filename);
            strip_extension(name);
            if (strcmp(name, prgm_name[n]) == 0)
            {
                print_dbg("\r\n found match...");
                print_dbg(name);
                fp = fl_fopen(p[n], "r+");
                prgmFileSize = dirent.size;
                break;
            }
            else
            {
                prgmFileSize = 0;
                fp = NULL;
            }
        }
    }
    else
    {
        print_dbg("\r\n dir error...");
        prgmFileSize = 0;
        fp = NULL;
    }
    */

    //  save to file
    fp = fl_fopen(p[n], "r+");
    prgmFileSize = N_TRK * sizeof(prgmTrack);

    fl_fwrite((const void*)prgmFileData, bytes, 1, fp);
    fl_fclose(fp);
//    fl_closedir(&dirstat);
    
    free_mem(prgmFileData);
    
    app_resume();
    
    return 0;
}

uint8_t files_load_npdx(char *p[], u8 n) {
    void *fp;
    
    ParamValueSwap tmp;
    u32 bytes = 0;
    uint8_t t, s;
    
    app_pause();
    
    //  load from file
    fp = fl_fopen(p[n], "r");
    prgmFileSize = N_TRK * sizeof(prgmTrack);
    prgmFileData = (u8*)alloc_mem(FILE_SIZE);
    fake_fread(prgmFileData, prgmFileSize, fp);
    fl_fclose(fp);
    
    //  copy data
    for (t=0; t<N_TRK; t++)
    {
        for (s=0; s<SQ_LEN; s++)
        {
            trk_deck[t]->s[s] = prgmFileData[bytes];
            bytes++;
            
            tmp.asByte[0] = prgmFileData[bytes];
            bytes++;
            tmp.asByte[1] = prgmFileData[bytes];
            bytes++;
            tmp.asByte[2] = prgmFileData[bytes];
            bytes++;
            tmp.asByte[3] = prgmFileData[bytes];
            bytes++;
            trk_deck[t]->l[s] = tmp.asInt;
        }
        trk_deck[t]->len = prgmFileData[bytes];
        bytes++;
    }
    
    //  no transfer to stm32, patterns are pre-loaded at startup...
    //  pattern change
    ctl_command_change(3, n, 0, 0, 0, 0, 0, 0);

    free_mem(prgmFileData);
    
    app_resume();
    
    return 0;
}

uint8_t files_save_npdx(char *p[], u8 n) {
    void *fp;
    
    ParamValueSwap tmp;
    u32 bytes = 0;
    uint8_t t, s;
    
    app_pause();
    
    prgmFileData = (u8*)alloc_mem(FILE_SIZE);
    
    //write pattern to buffer
    for (t=0; t<N_TRK; t++)
    {
        for (s=0; s<SQ_LEN; s++)
        {
            prgmFileData[bytes] = trk_deck[t]->s[s];
            bytes++;
            
            tmp.asInt = trk_deck[t]->l[s];
            prgmFileData[bytes] = tmp.asByte[0];
            bytes++;
            prgmFileData[bytes] = tmp.asByte[1];
            bytes++;
            prgmFileData[bytes] = tmp.asByte[2];
            bytes++;
            prgmFileData[bytes] = tmp.asByte[3];
            bytes++;
        }
        prgmFileData[bytes] = trk_deck[t]->len;
        bytes++;
    }
    
    //  save to file
    fp = fl_fopen(p[n], "r+");
    prgmFileSize = N_TRK * sizeof(prgmTrack);
    fl_fwrite((const void*)prgmFileData, bytes, 1, fp);
    fl_fclose(fp);
    
    free_mem(prgmFileData);
    
    app_resume();
    
    return 0;
}

uint8_t files_load_ncps(char *p[], u8 n) {
    void *fp;

    ParamValueSwap tmp;
    u32 bytes = 0;
    uint8_t s;
    
    app_pause();
    
    //  load from file
    fp = fl_fopen(p[n], "r");
    prgmFileSize = sizeof(prgmTrack);
    prgmFileData = (u8*)alloc_mem(FILE_SIZE);
    fake_fread(prgmFileData, prgmFileSize, fp);
    fl_fclose(fp);
    
    //  copy data
    for (s=0; s<SQ_LEN; s++)
    {
        //  pattern value check...
        if (prgmFileData[bytes] > n_pdx - 1)
        {
            //  if pattern number is above the number of patterns on card, default to first pattern
            composition->s[s] = 0;
        }
        else
        {
            composition->s[s] = prgmFileData[bytes];
        }
        bytes++;
        
        tmp.asByte[0] = prgmFileData[bytes];
        bytes++;
        tmp.asByte[1] = prgmFileData[bytes];
        bytes++;
        tmp.asByte[2] = prgmFileData[bytes];
        bytes++;
        tmp.asByte[3] = prgmFileData[bytes];
        bytes++;
        composition->l[s] = tmp.asInt;
    }
    composition->len = prgmFileData[bytes];
    bytes++;
    
    free_mem(prgmFileData);
    
    app_resume();
    
    return 0;
}

uint8_t files_save_ncps(char *p[], u8 n) {
    void *fp;

    ParamValueSwap tmp;
    u32 bytes = 0;
    uint8_t s;
    
    app_pause();
    
    prgmFileData = (u8*)alloc_mem(FILE_SIZE);
    
    //write pattern to buffer
    for (s=0; s<SQ_LEN; s++)
    {
        prgmFileData[bytes] = composition->s[s];
        bytes++;
            
        tmp.asInt = composition->l[s];
        prgmFileData[bytes] = tmp.asByte[0];
        bytes++;
        prgmFileData[bytes] = tmp.asByte[1];
        bytes++;
        prgmFileData[bytes] = tmp.asByte[2];
        bytes++;
        prgmFileData[bytes] = tmp.asByte[3];
        bytes++;
    }
    prgmFileData[bytes] = composition->len;
    bytes++;
    
    //  save to file
    fp = fl_fopen(p[n], "r+");
    prgmFileSize = sizeof(prgmTrack);
    fl_fwrite((const void*)prgmFileData, bytes, 1, fp);
    fl_fclose(fp);
    
    free_mem(prgmFileData);
    
    app_resume();
    
    return 0;
}

uint8_t files_load_nscn(char *p[], u8 n) {
    void *fp;
    
    ParamValueSwap tmp;
    u32 bytes = 0;
    uint8_t i, t;
    
    app_pause();
    
    //  load from file
    fp = fl_fopen(p[n], "r");
    prgmFileSize = sizeof(prgmScene);
    prgmFileData = (u8*)alloc_mem(FILE_SIZE);
    fake_fread(prgmFileData, prgmFileSize, fp);
    fl_fclose(fp);
    
    //  copy scene data
    //  chords
    for(i=0;i<N_CHORD;i++)
    {
        for(t=0;t<N_TRK;t++)
        {
            tmp.asByte[0] = prgmFileData[bytes];
            bytes++;
            tmp.asByte[1] = prgmFileData[bytes];
            bytes++;
            tmp.asByte[2] = prgmFileData[bytes];
            bytes++;
            tmp.asByte[3] = prgmFileData[bytes];
            bytes++;
            scene->chord[i].nf[t] = tmp.asInt;
        }
        
        for(t=0;t<N_TRK;t++)
        {
            tmp.asByte[0] = prgmFileData[bytes];
            bytes++;
            tmp.asByte[1] = prgmFileData[bytes];
            bytes++;
            tmp.asByte[2] = prgmFileData[bytes];
            bytes++;
            tmp.asByte[3] = prgmFileData[bytes];
            bytes++;
            scene->chord[i].nff[t] = tmp.asInt;
        }
        
        tmp.asByte[0] = prgmFileData[bytes];
        bytes++;
        tmp.asByte[1] = prgmFileData[bytes];
        bytes++;
        tmp.asByte[2] = prgmFileData[bytes];
        bytes++;
        tmp.asByte[3] = prgmFileData[bytes];
        bytes++;
        scene->chord[i].nt = tmp.asInt;
        
        for(t=0;t<N_TRK;t++)
        {
            tmp.asByte[0] = prgmFileData[bytes];
            bytes++;
            tmp.asByte[1] = prgmFileData[bytes];
            bytes++;
            tmp.asByte[2] = prgmFileData[bytes];
            bytes++;
            tmp.asByte[3] = prgmFileData[bytes];
            bytes++;
            scene->chord[i].nout[t] = tmp.asInt;
        }
    }
    
    free_mem(prgmFileData);
    
    app_resume();
    
    return 0;
}

uint8_t files_save_nscn(char *p[], u8 n) {
    void *fp;
    
    ParamValueSwap tmp;
    u32 bytes = 0;
    uint8_t i, t;
    
    app_pause();
    
    prgmFileData = (u8*)alloc_mem(FILE_SIZE);
    
    //  write pattern to buffer
    //  chords
    for(i=0;i<N_CHORD;i++)
    {
        for(t=0;t<N_TRK;t++)
        {
            tmp.asInt = scene->chord[i].nf[t];
            prgmFileData[bytes] = tmp.asByte[0];
            bytes++;
            prgmFileData[bytes] = tmp.asByte[1];
            bytes++;
            prgmFileData[bytes] = tmp.asByte[2];
            bytes++;
            prgmFileData[bytes] = tmp.asByte[3];
            bytes++;
        }

        for(t=0;t<N_TRK;t++)
        {
            tmp.asInt = scene->chord[i].nff[t];
            prgmFileData[bytes] = tmp.asByte[0];
            bytes++;
            prgmFileData[bytes] = tmp.asByte[1];
            bytes++;
            prgmFileData[bytes] = tmp.asByte[2];
            bytes++;
            prgmFileData[bytes] = tmp.asByte[3];
            bytes++;
        }
        
        tmp.asInt = scene->chord[i].nt;
        prgmFileData[bytes] = tmp.asByte[0];
        bytes++;
        prgmFileData[bytes] = tmp.asByte[1];
        bytes++;
        prgmFileData[bytes] = tmp.asByte[2];
        bytes++;
        prgmFileData[bytes] = tmp.asByte[3];
        bytes++;
        
        for(t=0;t<N_TRK;t++)
        {
            tmp.asInt = scene->chord[i].nout[t];
            prgmFileData[bytes] = tmp.asByte[0];
            bytes++;
            prgmFileData[bytes] = tmp.asByte[1];
            bytes++;
            prgmFileData[bytes] = tmp.asByte[2];
            bytes++;
            prgmFileData[bytes] = tmp.asByte[3];
            bytes++;
        }
    }
    
    //  save to file
    fp = fl_fopen(p[n], "r+");
    prgmFileSize = sizeof(prgmScene);
    fl_fwrite((const void*)prgmFileData, bytes, 1, fp);
    fl_fclose(fp);
    
    free_mem(prgmFileData);
    
    app_resume();
    
    return 0;
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
