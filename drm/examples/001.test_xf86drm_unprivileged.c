/*
 *	This file corresponds to the following section in xf86drm.h
 *	General user-level programmer's API: unprivileged.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <drm.h>
#include <xf86drm.h>

static void print_version (const char* prelog, drmVersionPtr dvp);

int 
main (int argc, char** argv)
{
    if (!drmAvailable ())
    {
	perror ("drm unavailable: ");
	exit (1);
    }
    int drm_fd = 0;
    if ((drm_fd = open ("/dev/dri/card0", O_RDWR|O_CLOEXEC)) < 0)
    {
	perror ("drm open failed:");
	
	exit (1);
    }
    //1. version (see xf86drm.c)
    drmVersionPtr dvp = NULL;
    dvp = drmGetVersion (drm_fd);
    print_version ("drmGetVersion:", dvp);
    drmFreeVersion (dvp);
    //2. lib version (see xf86drm.c)
    dvp = NULL;
    dvp = drmGetLibVersion (drm_fd);
    print_version ("drmGetLibVersion:", dvp);
    drmFreeVersion (dvp);
    //3. Capability (see struct drm_get_cap and DRM_CAP_* in drm.h
    //    drmGetCap see xf86drm.c
#define print_cap(fd, CAP, pval)\
    if (drmGetCap (fd, CAP, pval)==0) \
	printf ("\t"#CAP": %d\n", *pval);\
    else\
	perror ("\t"#CAP": ");

    uint64_t value = 0;
    printf ("drmGetCap:\n");
    print_cap(drm_fd, DRM_CAP_DUMB_BUFFER, &value);
    print_cap(drm_fd, DRM_CAP_VBLANK_HIGH_CRTC, &value);
    print_cap(drm_fd, DRM_CAP_DUMB_PREFERRED_DEPTH, &value);
    print_cap(drm_fd, DRM_CAP_DUMB_PREFER_SHADOW, &value);
    print_cap(drm_fd, DRM_CAP_PRIME, &value);
    //print_cap (drm_fd, DRM_CAP_TIMESTAMP_MONOTONIC, &value);
    //4. DRM magic. drmGetMagic see xf86drm.c, ]
    //   drm_magic_t (unsigned int) see drm.h
    drm_magic_t magic;
    printf ("drmGetMagic: ");
    if (drmGetMagic (drm_fd, &magic)==0)
	printf ("%u", magic);
    printf ("\n");
    //5. DRM bus id. see xf86drm.c
    char* busid=NULL;
    busid = drmGetBusid (drm_fd);
    printf ("drmGetBusid: %s\n", busid);
    drmFreeBusid (busid);
    //6. drmGetInterruptFromBusID
    //7. drmGetMap
    //8. drmGetClient
    //9. drmGetStats
    drmStatsT stats;
    printf ("drmGetStats:\n");
    if(drmGetStats (drm_fd, &stats)==0)
    {
	printf("\tcount: %u\n", stats.count);
	int i=0;
	for (i=0; i<stats.count; i++)
	{
	    printf("\t#%d\n", i);
	    printf("\tvalue: %u\n", stats.data[i].value);
	    printf("\tlong format: %s\n", stats.data[i].long_format);
	    printf("\tlong name: %s\n", stats.data[i].long_name);
	    printf("\trate format: %s\n", stats.data[i].rate_format);
	    printf("\trate name: %s\n", stats.data[i].rate_name);
	    printf("\tisvalue: %d\n", stats.data[i].isvalue);
	    printf("\tmult names: %s\n", stats.data[i].mult_names);
	    printf("\tmult: %d\n", stats.data[i].mult);
	    printf("\tverbose: %d\n", stats.data[i].verbose);
	}
    }	
    //10. drmSetInterfaceVersion
    //11. drmCommandNone
    //12. drmCommandRead
    //13. drmCommandWrite
    //14. drmCommandWriteRead


    drmClose (drm_fd);
}

static void
print_version (const char* prelog, drmVersionPtr dvp)
{
    if (dvp == NULL)
	return;
    if (prelog != NULL)
	fprintf (stdout, "%s\n", prelog);
    fprintf (stdout, "\tversion_major: %d\n", dvp->version_major);
    fprintf (stdout, "\tversion_minor: %d\n", dvp->version_minor);
    fprintf (stdout, "\tversion_patchlevel: %d\n", dvp->version_patchlevel);
    fprintf (stdout, "\tdriver name  : %s (len : %d)\n", dvp->name, dvp->name_len);
    fprintf (stdout, "\tdate         : %s (len : %d)\n", dvp->date, dvp->date_len);
    fprintf (stdout, "\tdescription  : %s (len : %d)\n", dvp->desc, dvp->desc_len);
}
