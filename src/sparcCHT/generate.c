#include "sparc.h"

static void function_header(char *s1, PARAMETER *plist, char *s2) {
	if (s1 != STR_NULL)
		printf("\nfunction %s", s1);

	putchar('(');
	if (plist == PRM_NULL)
		goto CLOSE_PLIST;

	for (printf("%s", plist->name);
		plist->next != PRM_NULL;) {
		plist = plist->next;
		printf(", %s", plist->name);
	}

CLOSE_PLIST:
	puts(") {");
	if (s2 != STR_NULL)
		printf("\t%s\n", s2);
}

static void ba_code(char *ba, char *varname, PARAMETER *plist,
	TEXT *code)
{
	if (code == TXT_NULL)
		return;

	printf("\nfunction cht_rpt_%s__%s", ba, varname);
	function_header(STR_NULL, plist, STR_NULL);
	txt_print(code, stdout, 1);
	putchar('}');
	putchar('\n');
}

static void sort_code(void)
{
	VARIABLE *v;
	SORT *s;

	printf("\nfunction cht_rpt_describe_sort() {\n");
	for (s = sort_list; s != SRT_NULL; s = s->next) {
		printf("\tcht_rpt_sort_field[%d] = \"%s\"\n",
			s->var->sort, s->var->name);

		if (s->var->input)
			printf("\tcht_rpt_sort_type[%d] = "
				"cht_rpt_input_type[%d]\n",
				s->var->sort, s->var->input);
		else {
			printf("\tcht_rpt_sort_type[%d] = ",
				s->var->sort);
			if (s->var->type)
				printf("%d", s->var->type);
			else
				printf("cht_coltype[\"%s.%s\"]",
					s->var->column->table->name,
					s->var->column->column);
			putchar('\n');
		}

		printf("\tcht_rpt_sort_order[%d] = ", s->var->sort);
		switch (s->ord) {
		case ORDER_ASC:
			putchar('1');
			break;
		case ORDER_DSC:
			putchar('-');
			putchar('1');
			break;
		default:
			putchar('0');
			break;
		}

		putchar('\n');
	}

	putchar('}');
	putchar('\n');

	printf("\nfunction cht_rpt_sort_send() {\n");
	for (s = sort_list; s != SRT_NULL; s = s->next)
		printf("\tcht_rpt_new_value[%d] = %s\n",
			s->var->sort, s->var->name);

	printf("\treturn(cht_rpt_sort_check())\n"
		"}\n");

	for (s = sort_list; s != SRT_NULL; s = s->next) {
		ba_code("before", s->var->name, s->bplist, s->before);
		ba_code("after", s->var->name, s->aplist, s->after);
	}

	printf("\nfunction cht_rpt_after_check(what) {\n");
	for (s = sort_last; s != SRT_NULL; s = s->prev) {
		if (s->after == TXT_NULL)
			continue;

		printf("\tif (cht_rpt_ba_status[%d] == what) {\n"
			"\t\tcht_rpt_after__%s()\n"
			"\t\tcht_rpt_ba_status[%d] = 3\n"
			"\t}\n", s->var->sort, s->var->name,
			s->var->sort);
	}

	putchar('}');
	putchar('\n');

	printf("\nfunction cht_rpt_before_check() {\n");
	for (s = sort_list; s != SRT_NULL; s = s->next) {
		if (s->before != TXT_NULL) {
			printf("\tif (cht_rpt_ba_status[%d] != 1)\n",
				s->var->sort);
			printf("\t\tcht_rpt_before__%s()\n\n",
				s->var->name);
		}
	}

	for (s = sort_list; s != SRT_NULL; s = s->next)
		printf("\tcht_rpt_ba_status[%d] = 1\n", s->var->sort);

	putchar('}');
	putchar('\n');

	printf("\n## Η function `cht_rpt_reset_input' θέτει τις "
		"τιμές των\n"
		"## πεδίων του δευτερογενούς (ταξινομημένου) "
		"input από\n"
		"## το array που της περνάμε ως μια μοναδική "
		"παράμετρο\n"
		"## Πρόκειται για ένα από τα δύο arrays: "
		"`cht_rpt_new_value',\n"
		"## `cht_rpt_sort_value'.\n\n"
		"function cht_rpt_reset_input(val_list) {\n");
	for (v = var_list; v != VAR_NULL; v = v->next) {
		if (v->sort)
			printf("\t%s = val_list[%d]\n",
				v->name, v->sort);
	}

	putchar('}');
	putchar('\n');

	printf("\n## Η function `cht_rpt_set_sort' κρατάει τις τιμές "
		"των\n"
		"## πεδίων του δευτερογενούς (ταξινομημένου) input "
		"στο array\n"
		"## `cht_rpt_sort_value' για να ελέγξει τις "
		"αλλαγές κατά την\n"
		"## παραλαβή του επόμενου δευτερογενούς input line.\n\n"
		"function cht_rpt_set_sort() {\n");
	for (v = var_list; v != VAR_NULL; v = v->next) {
		if (v->sort)
			printf("\tcht_rpt_sort_value[%d] = %s\n",
				v->sort, v->name);
	}

	putchar('}');
	putchar('\n');
}

static void print_script_name(void)
{
	char *p;
	char *s;

	printf("\tcht_rpt_script_name = \"");
	if (script_name == STR_NULL) {
		if (source != STR_NULL) {
			cht_error(STR_NULL);
			fprintf(stderr, "script name set to `%s'\n",
				(script_name = source));
		}
	}

	if (script_name != STR_NULL)
		cs_print(script_name);
	else {
		cht_error(STR_NULL);
		fprintf(stderr, "WARNING: undefined script name\n");
	}

	printf("\"\n\n"
		"\tcht_rpt_begin_code()\n"
		"\tcht_rpt_before_report()\n"
		"}\n");
}

void generate(void)
{
	VARIABLE *v;

	printf("\n## Εδώ παρατίθεται αυτούσιος ο κώδικας του "
		"`functions' section.\n");
	txt_print(functions_code, stdout, 0);

	function_header("cht_rpt_begin_code", begin_plist,
		"## Εδώ παρατίθεται ο κώδικας του `begin' section.");
	txt_print(begin_code, stdout, 1);
	putchar('}');
	putchar('\n');

	printf("\nfunction cht_rpt_describe_input(err) {\n"
		"\t## Εδώ γίνεται η περιγραφή του input.\n");
	if (tbl_list != TBL_NULL)
		table_columns();

	printf("\tcht_abort(err)\n");
	printf("\n\t## Τίθενται τα ονόματα και τα data types\n"
		"\t## των input fields.\n");
	for (v = var_list; v != VAR_NULL; v = v->next) {
		if (!(v->input))
			continue;

		printf("\tcht_rpt_input_field[%d] = \"%s\"\n"
			"\tcht_rpt_input_type[%d] = ", v->input,
			v->name, v->input);
		if (v->type)
			printf("%d", v->type);
		else
			printf("cht_coltype[\"%s.%s\"]",
				v->column->table->name,
				v->column->column);

		putchar('\n');
	}

	putchar('}');
	putchar('\n');

	printf("\nfunction cht_rpt_check_input(offset,\t\terrs) {\n"
		"\t## Γίνεται έλεγχος στην ορθότητα των τιμών\n"
		"\t## των πεδίων του input.\n"
		"\terrs = 0\n");
	for (v = var_list; v != VAR_NULL; v = v->next) {
		if (v->input)
			printf("\terrs += cht_rpt_check_value"
				"(%d, offset)\n", v->input);
	}

	printf("\treturn(errs)\n"
		"}\n");

	printf("\nfunction cht_rpt_set_input(offset) {\n"
		"\t## Θέτω τις τιμές των input fields.\n");
	for (v = var_list; v != VAR_NULL; v = v->next) {
		if (v->input)
			printf("\t%s = cht_rpt_set_value"
				"(%d, offset)\n", v->name, v->input);
	}

	printf("\treturn(0)\n}\n");

	function_header("cht_rpt_input_code", input_plist,
		"## Παρατίθεται ο κώδικας του `input' section.");
	txt_print(input_code, stdout, 1);
	printf("\n\treturn(0)\n"
		"}\n");

	function_header("cht_rpt_before_first_data_code",
		before_first_data_plist,
		"## Παρατίθεται ο κώδικας του `before any data' "
			"section.");
	txt_print(before_first_data_code, stdout, 1);
	putchar('}');
	putchar('\n');

	if ((after_sort_code != TXT_NULL) && ((!n_sort) || sorted)){
		cht_error("warning: `after sort' code "
			"appended to `before data set'");
		before_data_code = txt_more(before_data_code,
			after_sort_code);
		after_sort_code = TXT_NULL;
	}

	function_header("cht_rpt_before_report_code",
		before_report_plist,
		"## Παρατίθεται ο κώδικας του `before report' "
			"section.");
	txt_print(before_report_code, stdout, 1);
	putchar('}');
	putchar('\n');

	function_header("cht_rpt_before_accepted_code",
		before_accepted_data_plist,
		"## Παρατίθεται ο κώδικας του `before any accepted "
			"data' section.");
	txt_print(before_accepted_data_code, stdout, 1);
	putchar('}');
	putchar('\n');

	function_header("cht_rpt_before_data_code", before_data_plist,
		"## Παρατίθεται ο κώδικας του `before data' "
			"section.");
	txt_print(before_data_code, stdout, 1);
	putchar('}');
	putchar('\n');

	function_header("cht_rpt_after_sort_code", after_sort_plist,
		"## Παρατίθεται ο κώδικας του `after sort'"
			"section.");
	txt_print(after_sort_code, stdout, 1);
	putchar('}');
	putchar('\n');

	function_header("cht_rpt_detail_code", detail_plist,
		"## Παρατίθεται ο κώδικας του `detail'"
			"section.");
	txt_print(detail_code, stdout, 1);
	putchar('}');
	putchar('\n');

	function_header("cht_rpt_after_data_code", after_data_plist,
		"## Παρατίθεται ο κώδικας του `after data'"
			"section.");
	txt_print(after_data_code, stdout, 1);
	putchar('}');
	putchar('\n');

	function_header("cht_rpt_after_report_code", after_report_plist,
		"## Παρατίθεται ο κώδικας του `after report' section.");
	txt_print(after_report_code, stdout, 1);
	putchar('}');
	putchar('\n');

	function_header("cht_rpt_after_accepted_code",
		after_accepted_data_plist,
		"## Παρατίθεται ο κώδικας του `after any accepted "
			"data' section.");
	txt_print(after_accepted_data_code, stdout, 1);
	putchar('}');
	putchar('\n');

	function_header("cht_rpt_after_last_data_code",
		after_last_data_plist,
		"## Παρατίθεται ο κώδικας του `after any data' "
			"section.");
	txt_print(after_last_data_code, stdout, 1);
	putchar('}');
	putchar('\n');

	function_header("cht_rpt_end_code", end_plist,
		"## Παρατίθεται ο κώδικας του `end' section.");
	txt_print(end_code, stdout, 1);
	putchar('}');
	putchar('\n');

	if (n_sort)
		sort_code();

	printf("\nBEGIN {\n"
		"\tcht_progname = \"awkrpt\"\n"
		"\tcht_rpt_input_count = %d\n"
		"\tcht_rpt_sort_count = %d\n",
		n_input, n_sort);

	if (sorted)
		puts("\tcht_rpt_sorted = 1");

	print_script_name();
}
