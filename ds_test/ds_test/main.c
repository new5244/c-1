/*!
  \file main.c
  \brief


  created by Kim Youngmin (ymkim@huvitz.com)
  creation date : 수  7월 17 17:12:15 KST 2013

*/
#include <stdio.h>
#include <ssl_common.h>
#include <sec_common.h>

#include <ds_def.h>
#include <ds_parse.h>

#if 0
#include <wrapper.h>
#endif 

#if 0
static DRILL_SHAPE * make_ds(int record_count)
{
	DRILL_SHAPE *pds;

	pds = alloc_DRILL_SHAPE(record_count);
	if ( pds == NULL ) {
		printf("drill shape alloc error\n");
		return NULL;
	}

	pr_DRILL_SHAPE(pds, 1);

	return pds;
}

static void ds_test(void)
{
	DRILL_SHAPE_LIST list;
	DRILL_SHAPE *pds;

	ssl_set_alloc_func( (void*)malloc, (void*)free);

	printf("tester\n");

	init_DRILL_SHAPE_LIST(&list);

	pds = make_ds(10);
	add_DRILL_SHAPE_to_list(&list, pds);
	pds = make_ds(3);
	add_DRILL_SHAPE_to_list(&list, pds);
	pds = make_ds(49);
	add_DRILL_SHAPE_to_list(&list, pds);
	pds = make_ds(7);
	add_DRILL_SHAPE_to_list(&list, pds);

	printf("list count = %d\n", get_DRILL_SHAPE_LIST_count(&list));



	pds = NULL;

	pds = get_DRILL_SHAPE_from_list(&list, 1);

	if ( pds ) {
		pr_DRILL_SHAPE(pds, 1);
	}

	free_DRILL_SHAPE_LIST(&list);
}

static void d2h_test(float angle)
{
	printf("dcs %.2f = %.4f ( %.2f ) \n", 
		angle,
		dcs_angle_2_huvitz_rad(angle),
		SAIBI_RAD2DEGREE( dcs_angle_2_huvitz_rad(angle)) );
}

static void h2d_test(float rad)
{
	printf("huvitz %.4f ( %.2f ) = %.2f\n", 
		rad,
		SAIBI_RAD2DEGREE( rad ), 
		huvitz_rad_2_dcs_angle( rad ) );
}


static void p_test(void)
{
	char buf[1024] = 
	"REM=HVHF=1;3;U;R;F;H\n"
	"REM=HVHR=1000;2000;3000\n"
	"REM=HVHA=0;1;2\n"

	"REM=HVHF=1;10;U;R;F;H\n"
	"REM=HVHR=1000;2000;3000\n"
	"REM=HVHA=0;1;2\n"

	"REM=HVHF=1;5;U;R;F;D\n"
	"REM=HVHR=11000;12000;13000;12000;11000\n"
	"REM=HVHA=1011;1515;2020;3000;4000\n"

	"REM=HVHF=1;4;U;R;F;D\n"
	"REM=HVHR=11000;12000;13000;12000;11000\n"
	"REM=HVHA=1011;1515;2020;3000;4000\n"

	"REM=HVHF=1;10;U;R;F\n"
	"REM=HVHR=13500;13600;13500;13500;13500;13500;13500;13500;13500;13500\n"
	"REM=HVHA=0;1000;2000;3000;4000;5000;6000;7000;8000;9000\n"

	"REM=HVHF=2;5;U;R;F\n"
	"REM=HVHR=13500;13600;13500;13500;13500;13500\n"

	"REM=HVHF=3;7;U;R;F;H\n"

	"REM=HVHF=4;9;U;R;F;H\n"
	"REM=HVHR=11000;12000;13000;12000;11000\n"

	"REM=HVHF=5;10;U;R;F;H\n"
	"REM=HVHA=0;1;2\n"

	"REM=HVHF=1;6;U;R;F\n"
	"REM=HVHR=13500;13600;13500;13500;13500;13500\n"
	"REM=HVHA=0;1000;2000;3000;4000;5000\n";

	DRILL_SHAPE_LIST shape_list;
	int shape_count = 0;
	DRILL_SHAPE *pds;
	int i;

	init_DRILL_SHAPE_LIST(&shape_list);

	replace_char(buf, '\n', CHAR_CR);

	shape_count = iso16284_to_DRILL_SHAPE_LIST(buf, &shape_list);

	printf("count = %d\n", shape_count);
	printf("list count = %d\n", get_DRILL_SHAPE_LIST_count(&shape_list));

	for ( i = 0 ; i < get_DRILL_SHAPE_LIST_count(&shape_list); i++ ) {
		pds = get_DRILL_SHAPE_from_list(&shape_list, i);
		if ( pds ) {
			pr_DRILL_SHAPE(pds, 1);
		}
	}
	free_DRILL_SHAPE_LIST(&shape_list);
}

#endif 


/// LF->CR
/// \param packet (in/out)
/// \param packet_len 
static void conv_packet_LF_to_CR(char *packet, int packet_len)
{
	int i;

	for ( i = 0 ; i < packet_len; i++) {
		if ( packet[i] == CHAR_LF ) {
			packet[i] = CHAR_CR;
			PRINTF("<LF>\n");
		} else if ( packet[i] == CHAR_CR ) {
			PRINTF("<CR>\n");
		} else if ( packet[i] == CHAR_RS ) {
			PRINTF("<RS>\n");
		} else if ( packet[i] == CHAR_FS ) {
			PRINTF("<FS>\n");
		} else if ( packet[i] == CHAR_GS ) {
			PRINTF("<GS>\n");
		} else if ( packet[i] >= ' ' && packet[i] <= 127 ) {
			PRINTF("%c", packet[i]);
		}
	}
}

static int conv_packet_CRLF_to_CR(char *packet, int packet_len)
{
#if 0
	int i;

	for ( i = 0 ; i < packet_len -1; i++) {
		if ( packet[i] == CHAR_CR && packet[i+1] == CHAR_LF ) {
			packet[i+1] = ' ';
			i++;
		} 
	}
#endif 
	int i, k;
	char *src, *dest;

	src = packet;
	dest = packet;

	for ( i = 0, k = 0 ; i < packet_len -1; i++) {
		if ( src[i] == CHAR_CR && src[i+1] == CHAR_LF ) {
			dest[k++] = src[i];
			i++;
		} else {
			dest[k++] = src[i];
		}
	}
	dest[k] = src[i];

	for ( i = k+1; i < packet_len ; i++) 
		dest[i] = 0;

	return k;
}

/// escape packed data
/// \param dest (out)
/// \param src
/// \param src_size
/// \return escaped_size
static int escape(char *dest, char *src, int src_size)
{
	int i, k;

	for ( i = 0, k = 0 ; i < src_size ; i++, k++ ) {
		if ( ( src[i] < 32 ) && vca_dcs_is_reserved_char( src[i] ) ) {
			dest[k++] = CHAR_ESC;
			dest[k] = src[i] | 0x80;
		} else {
			dest[k] = src[i];
		}
	}
	return k;
}           


static void test_dcs_parse(void)
{
	char escaped_buf[3072] = {0, };

char bin_packet[] = {
0x4a, 0x4f, 0x42, 0x3d, 0x22, 0x37,
0x33, 0x31, 0x37, 0x34, 0x22, 0x0d, 0x0a, 0x53, 0x54, 0x41, 0x54, 0x55, 0x53, 0x3d, 0x30, 0x0d,
0x0a, 0x43, 0x49, 0x52, 0x43, 0x3d, 0x31, 0x34, 0x39, 0x2e, 0x39, 0x38, 0x3b, 0x31, 0x34, 0x39,
0x2e, 0x39, 0x38, 0x0a, 0x46, 0x43, 0x52, 0x56, 0x3d, 0x35, 0x2e, 0x30, 0x30, 0x3b, 0x35, 0x2e,
0x30, 0x30, 0x0a, 0x46, 0x54, 0x59, 0x50, 0x3d, 0x32, 0x0a, 0x44, 0x42, 0x4c, 0x3d, 0x32, 0x30,
0x2e, 0x30, 0x30, 0x0a, 0x49, 0x50, 0x44, 0x3d, 0x33, 0x32, 0x2e, 0x35, 0x3b, 0x33, 0x32, 0x2e,
0x35, 0x0a, 0x4e, 0x50, 0x44, 0x3d, 0x33, 0x30, 0x2e, 0x35, 0x3b, 0x33, 0x30, 0x2e, 0x35, 0x0a,
0x4c, 0x54, 0x59, 0x50, 0x3d, 0x54, 0x52, 0x2c, 0x46, 0x54, 0x3b, 0x54, 0x52, 0x2c, 0x46, 0x54,
0x0a, 0x4f, 0x43, 0x48, 0x54, 0x3d, 0x32, 0x36, 0x2e, 0x30, 0x3b, 0x32, 0x36, 0x2e, 0x30, 0x0a,
0x53, 0x45, 0x47, 0x48, 0x54, 0x3d, 0x32, 0x33, 0x2e, 0x30, 0x3b, 0x32, 0x33, 0x2e, 0x30, 0x0a,
0x56, 0x42, 0x4f, 0x58, 0x3d, 0x34, 0x34, 0x2e, 0x38, 0x36, 0x3b, 0x34, 0x34, 0x2e, 0x38, 0x36,
0x0a, 0x48, 0x42, 0x4f, 0x58, 0x3d, 0x34, 0x39, 0x2e, 0x33, 0x34, 0x3b, 0x34, 0x39, 0x2e, 0x33,
0x34, 0x0a, 0x46, 0x42, 0x46, 0x43, 0x49, 0x4e, 0x3d, 0x30, 0x2e, 0x30, 0x3b, 0x30, 0x2e, 0x30,
0x0a, 0x46, 0x42, 0x46, 0x43, 0x55, 0x50, 0x3d, 0x30, 0x2e, 0x30, 0x30, 0x3b, 0x30, 0x2e, 0x30,
0x30, 0x0a, 0x45, 0x54, 0x59, 0x50, 0x3d, 0x31, 0x0a, 0x4c, 0x4d, 0x41, 0x54, 0x54, 0x59, 0x50,
0x45, 0x3d, 0x36, 0x3b, 0x36, 0x0a, 0x42, 0x45, 0x56, 0x50, 0x3d, 0x3f, 0x3b, 0x3f, 0x0a, 0x42,
0x45, 0x56, 0x4d, 0x3d, 0x3f, 0x3b, 0x3f, 0x0a, 0x42, 0x45, 0x56, 0x43, 0x3d, 0x3f, 0x3b, 0x3f,
0x0a, 0x45, 0x50, 0x52, 0x45, 0x53, 0x53, 0x3d, 0x33, 0x3b, 0x33, 0x0a, 0x50, 0x49, 0x4e, 0x42,
0x3d, 0x30, 0x3b, 0x30, 0x0a, 0x50, 0x4f, 0x4c, 0x49, 0x53, 0x48, 0x3d, 0x30, 0x0a, 0x53, 0x57,
0x49, 0x44, 0x54, 0x48, 0x3d, 0x32, 0x38, 0x3b, 0x32, 0x38, 0x0a, 0x44, 0x4f, 0x3d, 0x42, 0x0a,
0x47, 0x44, 0x4f, 0x3d, 0x42, 0x0a, 0x54, 0x52, 0x43, 0x46, 0x4d, 0x54, 0x3d, 0x33, 0x3b, 0x35,
0x30, 0x30, 0x3b, 0x45, 0x3b, 0x52, 0x3b, 0x46, 0x0a, 0x52, 0x3d, 0x77, 0x09, 0x1b, 0x86, 0x05,
0x1b, 0x86, 0x05, 0x1b, 0x86, 0x1b, 0x86, 0x07, 0x1b, 0x86, 0x05, 0x1b, 0x86, 0x05, 0x07, 0x1b,
0x86, 0x1b, 0x86, 0x05, 0x1b, 0x86, 0x1b, 0x86, 0x1b, 0x86, 0x1b, 0x86, 0x08, 0x05, 0x04, 0x04,
0x05, 0x04, 0x04, 0x04, 0x04, 0x03, 0x02, 0x03, 0x03, 0x01, 0x00, 0x01, 0x00, 0x01, 0xff, 0x00,
0xfe, 0xff, 0xfe, 0xfd, 0xfc, 0xfc, 0xfc, 0xfb, 0xfb, 0xfc, 0xfa, 0xfb, 0xfa, 0xf9, 0xfb, 0xfa,
0xf9, 0xfa, 0xfa, 0xf9, 0xfa, 0xfa, 0xf9, 0xfa, 0xf8, 0xfb, 0xf9, 0xfa, 0xf9, 0xfa, 0xfa, 0xf9,
0xfa, 0xf9, 0xfb, 0xfa, 0xf9, 0xfa, 0xfa, 0xfa, 0xfb, 0xfa, 0xfb, 0xfc, 0xfa, 0xfb, 0xfa, 0xfc,
0xfb, 0xfc, 0xfd, 0xfa, 0xfc, 0xfc, 0xfb, 0xfd, 0xfc, 0xfd, 0xfc, 0xfd, 0xfd, 0xfc, 0xfe, 0xfd,
0xfe, 0xfe, 0xfe, 0xff, 0xfd, 0xfe, 0xfe, 0xfd, 0x00, 0xfe, 0xff, 0xff, 0xff, 0x00, 0xff, 0x00,
0x00, 0xff, 0x00, 0x00, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x02, 0x01,
0x01, 0x02, 0x02, 0x02, 0x01, 0x03, 0x02, 0x02, 0x02, 0x03, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
0x03, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x05, 0x1b, 0x86, 0x05, 0x03, 0x04, 0x1b, 0x86, 0x1b,
0x86, 0x1b, 0x86, 0x09, 0x05, 0x07, 0x05, 0x07, 0x08, 0x08, 0x1b, 0x86, 0x07, 0x07, 0x08, 0x07,
0x07, 0x07, 0x07, 0x07, 0x08, 0x07, 0x07, 0x07, 0x08, 0x08, 0x07, 0x08, 0x05, 0x08, 0x07, 0x1b,
0x86, 0x1b, 0x86, 0x05, 0x07, 0x1b, 0x86, 0x04, 0x04, 0x04, 0x05, 0x04, 0x01, 0x02, 0x02, 0x00,
0x01, 0x01, 0x00, 0x00, 0x00, 0x03, 0x02, 0x04, 0x01, 0xff, 0xfc, 0xfc, 0xfb, 0xfa, 0xfb, 0xf9,
0xfa, 0xf9, 0xf8, 0xf9, 0xf8, 0xf9, 0xfa, 0xf9, 0xfa, 0xfb, 0xf7, 0xf9, 0xfa, 0xf9, 0xfa, 0xfb,
0xf9, 0xfb, 0xfb, 0xfa, 0xfa, 0xfa, 0xfa, 0xfd, 0xfb, 0xfc, 0xfb, 0xfa, 0xfc, 0xfb, 0xfc, 0xfe,
0xfd, 0xfc, 0xfc, 0xfc, 0xfe, 0xfa, 0xfd, 0xfd, 0xfe, 0xfe, 0xfd, 0xfe, 0xfd, 0xff, 0xfe, 0xfe,
0xff, 0xfe, 0xff, 0xfe, 0xfe, 0xff, 0xff, 0xfe, 0x00, 0xfd, 0xff, 0xfe, 0xff, 0x00, 0xfe, 0x00,
0xff, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00,
0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0x01, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xfe,
0x01, 0xff, 0xff, 0x00, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xfd, 0xff, 0xfe, 0x00, 0xfe,
0xfe, 0xff, 0xfd, 0xff, 0xff, 0xfd, 0xfe, 0xfd, 0xfe, 0xfe, 0xfe, 0xfe, 0xfd, 0xfe, 0xfe, 0xfd,
0xff, 0xfd, 0xfe, 0xfe, 0xfd, 0xff, 0xfe, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xfe, 0xff, 0xfe, 0xfe,
0xfe, 0xfe, 0xfe, 0xff, 0xfe, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xfe, 0x00, 0xff, 0xfe, 0x00, 0xfe,
0xff, 0x00, 0xfe, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x01, 0x01, 0xff, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01,
0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x02, 0x00, 0x02, 0x00, 0x01, 0x01,
0x01, 0x00, 0x01, 0x00, 0x02, 0x01, 0x01, 0x01, 0x00, 0x02, 0x00, 0x01, 0x01, 0x01, 0x02, 0x02,
0x00, 0x01, 0x03, 0x01, 0x02, 0x02, 0x00, 0x03, 0x02, 0x02, 0x02, 0x02, 0x02, 0x01, 0x02, 0x02,
0x03, 0x03, 0x03, 0x03, 0x04, 0x03, 0x02, 0x04, 0x02, 0x04, 0x02, 0x05, 0x03, 0x04, 0x04, 0x04,
0x02, 0x03, 0x03, 0x04, 0x05, 0x04, 0x05, 0x04, 0x04, 0x04, 0x05, 0x04, 0x04, 0x05, 0x05, 0x05,
0x05, 0x05, 0x05, 0x01, 0x0d, 0x0a, 0x54, 0x52, 0x43, 0x46, 0x4d, 0x54, 0x3d, 0x33, 0x3b, 0x35,
0x30, 0x30, 0x3b, 0x45, 0x3b, 0x4c, 0x3b, 0x46, 0x0a, 0x52, 0x3d, 0x7b, 0x09, 0x05, 0x04, 0x05,
0x03, 0x1b, 0x86, 0x1b, 0x86, 0x1b, 0x86, 0x1b, 0x86, 0x05, 0x05, 0x07, 0x05, 0x1b, 0x86, 0x07,
0x1b, 0x86, 0x07, 0x09, 0x05, 0x1b, 0x86, 0x07, 0x1b, 0x86, 0x07, 0x08, 0x07, 0x08, 0x07, 0x1b,
0x86, 0x07, 0x05, 0x1b, 0x86, 0x05, 0x04, 0x04, 0x01, 0xff, 0xfc, 0xfe, 0xfd, 0x00, 0x00, 0x00,
0xff, 0xff, 0x00, 0xfe, 0xfe, 0xff, 0xfc, 0xfb, 0xfc, 0xfc, 0xfc, 0xfa, 0xf9, 0xfb, 0xfa, 0xfa,
0xf9, 0xf8, 0xfb, 0xf8, 0xf9, 0xf8, 0xf8, 0xf9, 0xf9, 0xf9, 0xf8, 0xf9, 0xf9, 0xf9, 0xf9, 0xf9,
0xf8, 0xf9, 0xf9, 0xfa, 0xf8, 0xf8, 0xf9, 0xfb, 0xf9, 0xfb, 0xf7, 0xfa, 0xfa, 0xfa, 0xfc, 0xfd,
0xfb, 0xfa, 0xfb, 0xfb, 0xfc, 0xfb, 0xfc, 0xfb, 0xfc, 0xfd, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc,
0xfd, 0xfe, 0xfe, 0xfe, 0xfd, 0xff, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xfe, 0x00, 0xff, 0x00, 0xfe,
0x00, 0x00, 0x00, 0x00, 0xff, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01,
0x02, 0x00, 0x03, 0x02, 0x02, 0x03, 0x01, 0x02, 0x02, 0x02, 0x03, 0x02, 0x04, 0x03, 0x03, 0x04,
0x03, 0x04, 0x03, 0x05, 0x04, 0x04, 0x1b, 0x86, 0x03, 0x04, 0x05, 0x04, 0x1b, 0x86, 0x05, 0x1b,
0x86, 0x04, 0x05, 0x1b, 0x86, 0x05, 0x1b, 0x86, 0x1b, 0x86, 0x1b, 0x86, 0x07, 0x1b, 0x86, 0x05,
0x07, 0x1b, 0x86, 0x07, 0x1b, 0x86, 0x1b, 0x86, 0x07, 0x1b, 0x86, 0x07, 0x05, 0x08, 0x1b, 0x86,
0x07, 0x1b, 0x86, 0x1b, 0x86, 0x07, 0x1b, 0x86, 0x1b, 0x86, 0x07, 0x1b, 0x86, 0x05, 0x07, 0x1b,
0x86, 0x05, 0x1b, 0x86, 0x04, 0x05, 0x05, 0x04, 0x04, 0x04, 0x03, 0x02, 0x01, 0x02, 0x00, 0x01,
0xff, 0x00, 0xff, 0x00, 0xff, 0xfd, 0xfd, 0xfe, 0xfd, 0xfc, 0xfc, 0xfc, 0xfc, 0xfb, 0xfc, 0xfc,
0xfb, 0xf8, 0xfa, 0xfa, 0xfa, 0xfa, 0xfb, 0xfa, 0xfa, 0xf9, 0xfb, 0xfa, 0xfb, 0xfa, 0xf9, 0xfa,
0xfa, 0xfb, 0xfa, 0xfb, 0xfa, 0xfa, 0xfd, 0xfb, 0xfb, 0xfb, 0xfb, 0xfb, 0xfb, 0xfc, 0xfc, 0xfb,
0xfc, 0xfc, 0xfc, 0xfb, 0xfc, 0xfb, 0xfc, 0xfd, 0xfd, 0xfe, 0xfc, 0xfc, 0xfc, 0xfd, 0xfb, 0xfe,
0xfc, 0xfe, 0xfc, 0xfe, 0xfd, 0xfc, 0xfd, 0xfd, 0xfd, 0xfd, 0xfe, 0xfe, 0xff, 0xfe, 0xfe, 0xfe,
0xfe, 0xfe, 0xfd, 0x00, 0xfe, 0xfe, 0xff, 0xfd, 0xff, 0x00, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0x00,
0xfe, 0x00, 0xff, 0xff, 0xff, 0xfe, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xfe, 0x00, 0xfe,
0x00, 0xff, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00,
0x00, 0x00, 0xff, 0x00, 0xff, 0x01, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x02, 0x00, 0x01, 0x02, 0x00, 0x02, 0x01, 0x00, 0x02,
0x02, 0x01, 0x02, 0x01, 0x02, 0x02, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x01, 0x02, 0x02, 0x01,
0x02, 0x01, 0x02, 0x02, 0x01, 0x03, 0x02, 0x02, 0x03, 0x01, 0x03, 0x02, 0x02, 0x03, 0x02, 0x02,
0x02, 0x02, 0x03, 0x02, 0x03, 0x01, 0x01, 0x03, 0x01, 0x02, 0x02, 0x00, 0x02, 0x01, 0x03, 0x02,
0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x00, 0x01, 0x01, 0xff, 0x02, 0x01, 0x00, 0x01, 0x01, 0x00,
0x01, 0xff, 0x01, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0xff, 0xff, 0x00,
0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x02, 0x00, 0x01, 0x02, 0x01,
0x03, 0x00, 0x02, 0x01, 0x01, 0x02, 0x02, 0x01, 0x02, 0x01, 0x02, 0x02, 0x01, 0x03, 0x02, 0x03,
0x02, 0x02, 0x03, 0x03, 0x1b, 0x86, 0x02, 0x04, 0x04, 0x04, 0x03, 0x02, 0x04, 0x05, 0x01, 0x0d,
0x0a, 0x1e, 0x43, 0x52, 0x43, 0x3d, 0x38, 0x33, 0x31, 0x38, 0x0d, 0x0a, 0x1d
};
	char packet_data[5000] =

	"ANS=3\r\n"
	"JOB=\"30924\"\r\n"
	"STATUS=0\r\n"
	"CIRC=145.63;145.63\r\n"
	"FCRV=4.10;4.20\r\n"
	"FTYP=2\r\n"
	"MPD=?;?\r\n"
	"DBL=18.04\r\n"
	"IPD=33.5;33.5\r\n"
	"NPD=0.0;0.0\r\n"
	"LTYP=SV;SV\r\n"
	"LTYPE=SV;SV\r\n"
	"OCHT=15.3;15.3\r\n"
	"SEGHT=0.0;0.0\r\n"
	"VBOX=30.64;30.64\r\n"
	"HBOX=53.76;53.76\r\n"
	"FBFCIN=0.0;0.0\r\n"
	"FBFCUP=0.00;0.00\r\n"
	"ETYP=1\r\n"
	"LMATTYPE=2;2\r\n"
	"BEVP=?;?\r\n"
	"BEVM=?;?\r\n"
	"BEVC=?;?\r\n"
	"EPRESS=3;3\r\n"
	"GWIDTH=?;?\r\n"
	"GDEPTH=?;?\r\n"
	"FPINB=0\r\n"
	"PINB=0;0\r\n"
	"PINBS=0\r\n"
	"POLISH=1\r\n"
	"BSIZ=?;?\r\n"
	"SWIDTH= 0; 0\r\n"
	"DO=B\r\n"
	"TRCFMT=1;400;E;R;F\r\n"
	"R=2632;2640;2649;2657;2666;2675;2684;2693;2703;2711;2720;2727;2736;2743;2750\r\n"
	"R=2755;2760;2763;2765;2766;2765;2762;2758;2750;2742;2733;2722;2708;2693;2675\r\n"
	"R=2656;2633;2611;2587;2562;2536;2509;2480;2451;2422;2392;2362;2332;2303;2272\r\n"
	"R=2242;2213;2184;2156;2128;2102;2076;2050;2025;2001;1977;1955;1933;1912;1892\r\n"
	"R=1873;1854;1836;1818;1802;1786;1770;1755;1742;1728;1715;1702;1690;1678;1667\r\n"
	"R=1657;1646;1637;1628;1619;1610;1601;1594;1587;1580;1574;1568;1562;1557;1552\r\n"
	"R=1548;1543;1539;1535;1532;1529;1527;1525;1523;1522;1521;1520;1520;1520;1520\r\n"
	"R=1520;1521;1522;1524;1526;1528;1530;1533;1536;1540;1544;1548;1553;1558;1563\r\n"
	"R=1569;1575;1582;1588;1595;1603;1611;1620;1629;1639;1649;1659;1670;1681;1693\r\n"
	"R=1705;1719;1732;1746;1761;1776;1792;1809;1826;1844;1863;1882;1902;1924;1946\r\n"
	"R=1969;1992;2017;2042;2068;2095;2124;2154;2184;2215;2249;2284;2319;2355;2392\r\n"
	"R=2430;2468;2504;2540;2574;2606;2635;2660;2683;2703;2718;2731;2740;2748;2754\r\n"
	"R=2756;2756;2757;2758;2756;2750;2743;2735;2727;2719;2714;2709;2705;2701;2697\r\n"
	"R=2694;2692;2690;2689;2688;2687;2687;2687;2688;2689;2689;2691;2693;2694;2696\r\n"
	"R=2699;2700;2703;2705;2706;2707;2709;2710;2711;2711;2710;2708;2706;2702;2696\r\n"
	"R=2688;2679;2666;2652;2633;2611;2586;2560;2529;2498;2465;2431;2396;2362;2329\r\n"
	"R=2298;2266;2236;2206;2177;2148;2121;2095;2070;2045;2021;1997;1975;1953;1932\r\n"
	"R=1911;1891;1872;1854;1836;1820;1803;1788;1772;1757;1742;1729;1716;1704;1692\r\n"
	"R=1680;1669;1659;1649;1639;1630;1621;1613;1605;1597;1591;1584;1578;1572;1567\r\n"
	"R=1561;1557;1552;1548;1544;1540;1537;1534;1532;1530;1528;1527;1526;1526;1527\r\n"
	"R=1528;1529;1531;1533;1534;1535;1538;1539;1542;1545;1548;1551;1556;1559;1564\r\n"
	"R=1569;1574;1579;1585;1592;1599;1606;1614;1621;1629;1638;1648;1658;1668;1678\r\n"
	"R=1690;1701;1713;1725;1739;1753;1767;1782;1798;1813;1830;1848;1867;1885;1904\r\n"
	"R=1924;1944;1965;1987;2010;2032;2055;2079;2104;2129;2155;2180;2206;2231;2257\r\n"
	"R=2282;2307;2331;2353;2376;2396;2415;2432;2448;2460;2471;2482;2490;2497;2503\r\n"
	"R=2509;2513;2516;2520;2523;2527;2530;2533;2537;2541;2545;2550;2554;2559;2564\r\n"
	"R=2569;2574;2580;2585;2591;2597;2604;2610;2617;2632\r\n"
	"TRCFMT=1;400;E;L;F\r\n"
	"R=2687;2688;2689;2690;2692;2694;2697;2701;2705;2709;2714;2719;2727;2735;2743\r\n"
	"R=2750;2756;2758;2757;2756;2756;2754;2748;2740;2731;2718;2703;2683;2660;2635\r\n"
	"R=2606;2574;2540;2504;2468;2430;2392;2355;2319;2284;2249;2215;2184;2154;2124\r\n"
	"R=2095;2068;2042;2017;1992;1969;1946;1924;1902;1882;1863;1844;1826;1809;1792\r\n"
	"R=1776;1761;1746;1732;1719;1705;1693;1681;1670;1659;1649;1639;1629;1620;1611\r\n"
	"R=1603;1595;1588;1582;1575;1569;1563;1558;1553;1548;1544;1540;1536;1533;1530\r\n"
	"R=1528;1526;1524;1522;1521;1520;1520;1520;1520;1520;1521;1522;1523;1525;1527\r\n"
	"R=1529;1532;1535;1539;1543;1548;1552;1557;1562;1568;1574;1580;1587;1594;1601\r\n"
	"R=1610;1619;1628;1637;1646;1657;1667;1678;1690;1702;1715;1728;1742;1755;1770\r\n"
	"R=1786;1802;1818;1836;1854;1873;1892;1912;1933;1955;1977;2001;2025;2050;2076\r\n"
	"R=2102;2128;2156;2184;2213;2242;2272;2303;2332;2362;2392;2422;2451;2480;2509\r\n"
	"R=2536;2562;2587;2611;2633;2656;2675;2693;2708;2722;2733;2742;2750;2758;2762\r\n"
	"R=2765;2766;2765;2763;2760;2755;2750;2743;2736;2727;2720;2711;2703;2693;2684\r\n"
	"R=2675;2666;2657;2649;2640;2632;2624;2617;2610;2604;2597;2591;2585;2580;2574\r\n"
	"R=2569;2564;2559;2554;2550;2545;2541;2537;2533;2530;2527;2523;2520;2516;2513\r\n"
	"R=2509;2503;2497;2490;2482;2471;2460;2448;2432;2415;2396;2376;2353;2331;2307\r\n"
	"R=2282;2257;2231;2206;2180;2155;2129;2104;2079;2055;2032;2010;1987;1965;1944\r\n"
	"R=1924;1904;1885;1867;1848;1830;1813;1798;1782;1767;1753;1739;1725;1713;1701\r\n"
	"R=1690;1678;1668;1658;1648;1638;1629;1621;1614;1606;1599;1592;1585;1579;1574\r\n"
	"R=1569;1564;1559;1556;1551;1548;1545;1542;1539;1538;1535;1534;1533;1531;1529\r\n"
	"R=1528;1527;1526;1526;1527;1528;1530;1532;1534;1537;1540;1544;1548;1552;1557\r\n"
	"R=1561;1567;1572;1578;1584;1591;1597;1605;1613;1621;1630;1639;1649;1659;1669\r\n"
	"R=1680;1692;1704;1716;1729;1742;1757;1772;1788;1803;1820;1836;1854;1872;1891\r\n"
	"R=1911;1932;1953;1975;1997;2021;2045;2070;2095;2121;2148;2177;2206;2236;2266\r\n"
	"R=2298;2329;2362;2396;2431;2465;2498;2529;2560;2586;2611;2633;2652;2666;2679\r\n"
	"R=2688;2696;2702;2706;2708;2710;2711;2711;2710;2709;2707;2706;2705;2703;2700\r\n"
	"R=2699;2696;2694;2693;2691;2689;2689;2688;2687;2687\r\n"
	"CRC=22172\r\n"
	"";

	char packet_data2[5000] =

	"ANS=3\n"
	"JOB=\"51769\"\n"
	"CIRC=158.03;158.03\n"
	"FCRV=3.50;3.50\n"
	"FTYP=1\n"
	"DBL=17.00\n"
	"IPD=30.0;30.0\n"
	"NPD=28.5;28.5\n"
	"LTYP=FT,BI;FT,BI\n"
	"OCHT=17.0;17.0\n"
	"SEGHT=13.0;13.0\n"
	"VBOX=33.98;33.98\n"
	"HBOX=58.49;58.49\n"
	"FBFCIN=0.0;0.0\n"
	"FBFCUP=0.00;0.00\n"
	"ETYP=1\n"
	"LMATTYPE=1;1\n"
	"BEVP=?;?\n"
	"BEVM=?;?\n"
	"BEVC=?;?\n"
	"EPRESS=3;3\n"
	"PINB=0;0\n"
	"POLISH=0\n"
	"SWIDTH=28;28\n"
	"DO=B\n"
	"GDO=B\n"
	"TRCFMT=1;400;E;R;F\n"
	"R=2785;2801;2816;2832;2848;2863;2877;2891;2905;2918;2933;2947;2963;2978;2993\n"
	"R=3005;3017;3026;3031;3033;3032;3028;3022;3009;2995;2977;2954;2927;2899;2869\n"
	"R=2839;2808;2777;2743;2710;2673;2640;2605;2571;2535;2502;2468;2436;2405;2376\n"
	"R=2345;2317;2289;2264;2240;2216;2190;2165;2141;2119;2097;2075;2057;2037;2019\n"
	"R=2001;1985;1968;1952;1936;1922;1907;1893;1881;1869;1857;1845;1834;1824;1814\n"
	"R=1803;1794;1785;1778;1770;1763;1757;1751;1745;1739;1734;1729;1724;1719;1715\n"
	"R=1712;1708;1705;1703;1700;1698;1698;1696;1697;1696;1696;1696;1697;1698;1701\n"
	"R=1703;1706;1708;1712;1716;1719;1724;1729;1734;1741;1745;1751;1759;1766;1773\n"
	"R=1782;1790;1799;1808;1817;1827;1838;1849;1862;1875;1888;1902;1918;1933;1949\n"
	"R=1965;1982;2000;2018;2036;2056;2076;2097;2118;2140;2162;2186;2212;2237;2264\n"
	"R=2291;2319;2348;2379;2410;2442;2475;2511;2544;2579;2618;2654;2690;2727;2763\n"
	"R=2798;2834;2869;2901;2932;2961;2987;3010;3026;3035;3043;3049;3051;3051;3048\n"
	"R=3043;3037;3030;3022;3013;3003;2995;2985;2977;2968;2961;2954;2947;2942;2936\n"
	"R=2931;2926;2921;2918;2914;2911;2908;2905;2901;2900;2897;2895;2893;2891;2890\n"
	"R=2890;2888;2888;2888;2888;2888;2888;2889;2890;2890;2891;2891;2892;2891;2891\n"
	"R=2888;2886;2880;2875;2865;2854;2839;2820;2796;2768;2739;2709;2675;2640;2603\n"
	"R=2567;2531;2496;2461;2426;2392;2360;2330;2301;2273;2247;2220;2195;2171;2148\n"
	"R=2126;2104;2083;2064;2044;2024;2006;1988;1969;1954;1939;1924;1909;1896;1881\n"
	"R=1869;1856;1844;1833;1823;1812;1803;1793;1784;1776;1767;1761;1754;1746;1741\n"
	"R=1735;1730;1725;1721;1717;1714;1710;1707;1703;1702;1701;1699;1698;1697;1697\n"
	"R=1698;1699;1698;1699;1700;1701;1704;1705;1710;1713;1716;1720;1724;1729;1734\n"
	"R=1738;1745;1751;1757;1766;1773;1783;1793;1802;1813;1822;1833;1844;1856;1868\n"
	"R=1880;1893;1907;1921;1936;1951;1967;1983;2000;2016;2035;2051;2069;2087;2106\n"
	"R=2125;2144;2162;2181;2198;2216;2233;2253;2271;2290;2307;2325;2344;2360;2377\n"
	"R=2393;2407;2419;2430;2443;2452;2461;2469;2477;2484;2491;2498;2503;2508;2514\n"
	"R=2519;2526;2533;2542;2549;2558;2566;2574;2584;2594;2604;2614;2624;2634;2645\n"
	"R=2656;2668;2680;2693;2704;2717;2730;2743;2756;2785\n"
	"TRCFMT=1;400;E;L;F\n"
	"R=2911;2914;2918;2921;2926;2931;2936;2942;2947;2954;2961;2968;2977;2985;2995\n"
	"R=3003;3013;3022;3030;3037;3043;3048;3051;3051;3049;3043;3035;3026;3010;2987\n"
	"R=2961;2932;2901;2869;2834;2798;2763;2727;2690;2654;2618;2579;2544;2511;2475\n"
	"R=2442;2410;2379;2348;2319;2291;2264;2237;2212;2186;2162;2140;2118;2097;2076\n"
	"R=2056;2036;2018;2000;1982;1965;1949;1933;1918;1902;1888;1875;1862;1849;1838\n"
	"R=1827;1817;1808;1799;1790;1782;1773;1766;1759;1751;1745;1741;1734;1729;1724\n"
	"R=1719;1716;1712;1708;1706;1703;1701;1698;1697;1696;1696;1696;1697;1696;1698\n"
	"R=1698;1700;1703;1705;1708;1712;1715;1719;1724;1729;1734;1739;1745;1751;1757\n"
	"R=1763;1770;1778;1785;1794;1803;1814;1824;1834;1845;1857;1869;1881;1893;1907\n"
	"R=1922;1936;1952;1968;1985;2001;2019;2037;2057;2075;2097;2119;2141;2165;2190\n"
	"R=2216;2240;2264;2289;2317;2345;2376;2405;2436;2468;2502;2535;2571;2605;2640\n"
	"R=2673;2710;2743;2777;2808;2839;2869;2899;2927;2954;2977;2995;3009;3022;3028\n"
	"R=3032;3033;3031;3026;3017;3005;2993;2978;2963;2947;2933;2918;2905;2891;2877\n"
	"R=2863;2848;2832;2816;2801;2785;2770;2756;2743;2730;2717;2704;2693;2680;2668\n"
	"R=2656;2645;2634;2624;2614;2604;2594;2584;2574;2566;2558;2549;2542;2533;2526\n"
	"R=2519;2514;2508;2503;2498;2491;2484;2477;2469;2461;2452;2443;2430;2419;2407\n"
	"R=2393;2377;2360;2344;2325;2307;2290;2271;2253;2233;2216;2198;2181;2162;2144\n"
	"R=2125;2106;2087;2069;2051;2035;2016;2000;1983;1967;1951;1936;1921;1907;1893\n"
	"R=1880;1868;1856;1844;1833;1822;1813;1802;1793;1783;1773;1766;1757;1751;1745\n"
	"R=1738;1734;1729;1724;1720;1716;1713;1710;1705;1704;1701;1700;1699;1698;1699\n"
	"R=1698;1697;1697;1698;1699;1701;1702;1703;1707;1710;1714;1717;1721;1725;1730\n"
	"R=1735;1741;1746;1754;1761;1767;1776;1784;1793;1803;1812;1823;1833;1844;1856\n"
	"R=1869;1881;1896;1909;1924;1939;1954;1969;1988;2006;2024;2044;2064;2083;2104\n"
	"R=2126;2148;2171;2195;2220;2247;2273;2301;2330;2360;2392;2426;2461;2496;2531\n"
	"R=2567;2603;2640;2675;2709;2739;2768;2796;2820;2839;2854;2865;2875;2880;2886\n"
	"R=2888;2891;2891;2892;2891;2891;2890;2890;2889;2888;2888;2888;2888;2888;2888\n"
	"R=2890;2890;2891;2893;2895;2897;2900;2901;2905;2911\n"
	"";

	VCA_DCS_FRAME_DATA dcs;
	int ret;
	int packet_len;

	SSL_PRINTF("dcs parse test\n");


	//replace_char(packet_data, '\n', CHAR_LF);
	SSL_PRINTF("org packet size = %d\n", strlen(packet_data));
	packet_len = conv_packet_CRLF_to_CR(packet_data, strlen(packet_data));
	SSL_PRINTF("packet size = %d\n", packet_len);
	conv_packet_LF_to_CR(packet_data, packet_len);
	//SSL_PRINTF("packet size = %d\n", strlen(packet_data));

	ret = vca_dcs_parse_frame_data(packet_data, packet_len, &dcs);
	SSL_PRINTF("ret = %d\n", ret);
	print_VCA_DCS_FRAME_DATA(&dcs, 1);
	print_VCA_DCS_DATA_RECORD(&dcs.data[0]);
	print_VCA_DCS_DATA_RECORD(&dcs.data[1]);






#if 0

	replace_char(packet_data2, '\n', CHAR_CR);
	SSL_PRINTF("packet size = %d\n", strlen(packet_data2));
	ret = vca_dcs_parse_frame_data(packet_data2, strlen(packet_data2), &dcs);
	SSL_PRINTF("ret = %d\n", ret);
	print_VCA_DCS_FRAME_DATA(&dcs, 1);
	print_VCA_DCS_DATA_RECORD(&dcs.data[0]);
	print_VCA_DCS_DATA_RECORD(&dcs.data[1]);


	SSL_PRINTF("bin_packet size = %d\n", sizeof(bin_packet));

	//SSL_PRINTF(bin_packet);
	print_iso16284_bin(bin_packet, sizeof(bin_packet));


	ret = vca_dcs_parse_frame_data(bin_packet, sizeof(bin_packet), &dcs);
	SSL_PRINTF("ret = %d\n", ret);

#endif 
}



int main(int argc, char *argv[])
{
	ssl_set_alloc_func( (void*)malloc, (void*)free);
#if 0
	int i = 0 ;

	for ( i = 0; i <= 360; i ++ ) {
		d2h_test(i);
	}

	for ( i = 0; i <= 360; i ++ ) {
		h2d_test( SAIBI_DEGREE2RAD ( i ) );
	}
	p_test();
#endif 

	test_dcs_parse();

	return 0;
}
/********** end of file **********/