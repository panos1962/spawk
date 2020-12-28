#include "sparc.h"

char *source = STR_NULL;
int lineno = 1;
int n_errs = 0;
DB_TABLE *tbl_list = TBL_NULL;
DB_COLUMN *col_list = COL_NULL;
VARIABLE *var_list = VAR_NULL;

TEXT *input_code = TXT_NULL;
TEXT *begin_code = TXT_NULL;
TEXT *before_report_code = TXT_NULL;
TEXT *before_first_data_code = TXT_NULL;
TEXT *before_accepted_data_code = TXT_NULL;
TEXT *before_data_code = TXT_NULL;
TEXT *detail_code = TXT_NULL;
TEXT *after_sort_code = TXT_NULL;
TEXT *after_data_code = TXT_NULL;
TEXT *after_last_data_code = TXT_NULL;
TEXT *after_report_code = TXT_NULL;
TEXT *after_accepted_data_code = TXT_NULL;
TEXT *end_code = TXT_NULL;
TEXT *functions_code = TXT_NULL;

PARAMETER *input_plist = PRM_NULL;
PARAMETER *begin_plist = PRM_NULL;
PARAMETER *before_report_plist = PRM_NULL;
PARAMETER *before_first_data_plist = PRM_NULL;
PARAMETER *before_accepted_data_plist = PRM_NULL;
PARAMETER *before_data_plist = PRM_NULL;
PARAMETER *detail_plist = PRM_NULL;
PARAMETER *after_sort_plist = PRM_NULL;
PARAMETER *after_data_plist = PRM_NULL;
PARAMETER *after_last_data_plist = PRM_NULL;
PARAMETER *after_report_plist = PRM_NULL;
PARAMETER *after_accepted_data_plist = PRM_NULL;
PARAMETER *end_plist = PRM_NULL;
PARAMETER *functions_plist = PRM_NULL;

VARIABLE *input_list = VAR_NULL;
SORT *sort_list = SRT_NULL;
SORT *sort_last = SRT_NULL;
int aacs_type = 0;
ARCHIVE *archive_list = ARC_NULL;
ARCHIVE *application_list = ARC_NULL;
int n_input = 0;
int n_sort = 0;
int sorted = -1;
int tpm_format = 0;
int readable_object = 1;
char *post_program = STR_NULL;
char *ante_program = STR_NULL;
char ante_option = 'a';
int remove_script = 0;
char *script_name = STR_NULL;
char *object_name = STR_NULL;
int plist_on = 0;
