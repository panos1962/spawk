#include "sparc.h"

char *progname = "sparc";
char *source = NULL;
unsigned int lineno = 0;
unsigned int error_count = 0;

unsigned int input_count = 0;
int sorted_input = -1;
char *script_name = STR_NULL;
int plist_on = 0;

SORT *sort_list = NULL;
TEXT *functions_code = NULL;
TEXT *input_code = NULL;
TEXT *begin_code = NULL;
TEXT *before_report_code = NULL;
TEXT *before_first_data_code = NULL;
TEXT *before_accepted_data_code = NULL;
TEXT *before_data_code = NULL;
TEXT *detail_code = NULL;
TEXT *after_sort_code = NULL;
TEXT *after_data_code = NULL;
TEXT *after_last_data_code = NULL;
TEXT *after_report_code = NULL;
TEXT *after_accepted_data_code = NULL;
TEXT *end_code = NULL;
PARAMETER *input_plist = NULL;
PARAMETER *begin_plist = NULL;
PARAMETER *before_report_plist = NULL;
PARAMETER *before_first_data_plist = NULL;
PARAMETER *before_accepted_data_plist = NULL;
PARAMETER *before_data_plist = NULL;
PARAMETER *detail_plist = NULL;
PARAMETER *after_sort_plist = NULL;
PARAMETER *after_data_plist = NULL;
PARAMETER *after_last_data_plist = NULL;
PARAMETER *after_report_plist = NULL;
PARAMETER *after_accepted_data_plist = NULL;
PARAMETER *end_plist = NULL;
