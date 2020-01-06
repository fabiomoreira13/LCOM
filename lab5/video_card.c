// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <stdio.h>

static char *video_mem;		/* Process (virtual) address to which VRAM is mapped */
static vbe_mode_info_t mem_info;
static unsigned h_res;	        /* Horizontal resolution in pixels */
static unsigned v_res;	        /* Vertical resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */


int vg_set_mode(uint16_t mode){ //test for 0x105 fails because get_mode_info returns 0 when it shouldnt. When we implement our own function, we must check and return something other than 0

    memset(&mem_info, 0, sizeof(mem_info));	/* zero the structure */
    mmap_t x;
    x.phys = 0;
    x.size = 1024*1024;
    lm_alloc(1024*1024,&x );
    if (vbe_get_mode_info(mode,&mem_info) != 0) {
        printf("Failed to get vbe info\n");
        return 1;
    }


    uint8_t bytesPerPixel;   //Must make sure enough memory is allocated. If bitsperpixel == 15, we need 2 bytes. if bitsperpixel == 24, we need 3.
    if (mem_info.BitsPerPixel %8 == 0){
        bytesPerPixel = mem_info.BitsPerPixel /8;
    }
    else bytesPerPixel = (mem_info.BitsPerPixel / 8) + 1;
    int r;
    struct minix_mem_range mr;

    unsigned int vram_base = mem_info.PhysBasePtr;  /* VRAM's physical addresss */

    unsigned int vram_size = mem_info.XResolution * mem_info.YResolution;  /* VRAM's size, but you can use
				    the frame-buffer size, instead */

    h_res = mem_info.XResolution;
    v_res = mem_info.YResolution;
    bits_per_pixel = mem_info.BitsPerPixel;

    /* Allow memory mapping */

    mr.mr_base = (phys_bytes) vram_base;
    mr.mr_limit = mr.mr_base + vram_size;

    if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
        panic("sys_privctl (ADD_MEM) failed: %d\n", r);

    /* Map memory */

    video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

    if(video_mem == MAP_FAILED)
        panic("couldn't map video memory");




    struct reg86 r86;
    memset(&r86, 0, sizeof(r86));	/* zero the structure */


    r86.intno = 0x010;
    r86.ah = 0x4F;
    r86.al = 0x02;
    r86.bx = BIT(14) | mode;    //Mode must be passed in the BX register.

    /* Make the BIOS call */

    if( sys_int86(&r86) != OK ) {
        printf("\tvg_exit(): sys_int86() failed \n");
        return 1;
    }

    return 0;
}


int (vg_draw_rectangle) (uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
    for (uint16_t i = 0; i < height; i++) {
        if (vg_draw_hline(x, y + i, width, color))
            if (vg_exit())
                return 1;

    }
    return 0;
}


int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color){
    int bytes_per_pixel = (int)((bits_per_pixel + 7) / 8);
    if (x>v_res)
        return 1;
//>>>>>a vram é de 32 bits certo????
    char* pixel_address=video_mem+(x+y*h_res)*bytes_per_pixel; //y*hres makes it skip x lines

    while (len>0){
        uint32_t color_copy=color;//color_will be changed in the for cycle
        for (unsigned int pixel_bit=0;pixel_bit<(bits_per_pixel/8);pixel_bit++){
//>>>>>>O número de pixeis da cor não deveria ser variável????
            *pixel_address=(uint8_t) color_copy;
            color_copy>>=8;
            pixel_address++;
        }
        len--;
    }
    return 0;

}






