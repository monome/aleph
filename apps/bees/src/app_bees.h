#ifndef _ALEPH_BEES_APP_BEES_H_
#define _ALEPH_BEES_APP_BEES_H_


#ifndef MIN
#define MIN 0
#endif
#ifndef MAJ
#define MAJ 0
#endif
#ifndef REV
#define REV 0
#endif
#ifndef VERSIONSTRING
#define VERSIONSTRING "none"
#endif


typedef struct _AppVersion { 
  u8 maj;
  u8 min; 
  u16 rev;
} AppVersion;

const AppVersion  beesVersion;

#endif
