/***************************************************************
 *
 * Copyright (C) 1990-2007, Condor Team, Computer Sciences Department,
 * University of Wisconsin-Madison, WI.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); you
 * may not use this file except in compliance with the License.  You may
 * obtain a copy of the License at
 * 
 *    http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ***************************************************************/

/* Test the UserPolicy implementation.
 */

#include "condor_common.h"
#include "condor_debug.h"
#include "condor_config.h"
#include "function_test_driver.h"
#include "unit_test_utils.h"
#include "emit.h"
#include "user_job_policy.h"

#define CLEANUP \
	classad_string.clear(); \
	delete ad

using namespace std;

#ifdef WANT_CLASSAD_NAMESPACE
using namespace classad;
#endif

static bool test_init_null(void);
static bool test_init_empty(void);
static bool test_init_non_empty(void);
static bool test_init_non_empty_miss1(void);
static bool test_init_non_empty_miss2(void);
static bool test_analyze_policy_no_init(void);
static bool test_analyze_policy_invalid_mode(void);
static bool test_analyze_policy_missing_job_status(void);
static bool test_analyze_policy_empty(void);
static bool test_analyze_policy_undefined_timer_remove(void);
static bool test_analyze_policy_undefined_periodic_hold(void);
static bool test_analyze_policy_undefined_periodic_release(void);
static bool test_analyze_policy_undefined_periodic_remove(void);
static bool test_analyze_policy_undefined_on_exit_hold(void);
static bool test_analyze_policy_undefined_on_exit_remove(void);
static bool test_analyze_policy_only_timer_remove(void);
static bool test_analyze_policy_only_periodic_hold(void);
static bool test_analyze_policy_only_periodic_hold_false(void);
static bool test_analyze_policy_only_periodic_release(void);
static bool test_analyze_policy_only_periodic_release_false(void);
static bool test_analyze_policy_only_periodic_remove(void);
static bool test_analyze_policy_only_false(void);
static bool test_analyze_policy_exit_missing(void);
static bool test_analyze_policy_exit_missing_both(void);
static bool test_analyze_policy_exit_missing_signal(void);
static bool test_analyze_policy_exit_missing_code(void);
static bool test_analyze_policy_exit_timer_remove(void);
static bool test_analyze_policy_exit_periodic_hold(void);
static bool test_analyze_policy_exit_periodic_hold_false(void);
static bool test_analyze_policy_exit_periodic_release(void);
static bool test_analyze_policy_exit_periodic_release_false(void);
static bool test_analyze_policy_exit_periodic_remove(void);
static bool test_analyze_policy_exit_on_exit_hold(void);
static bool test_analyze_policy_exit_on_exit_remove(void);
static bool test_analyze_policy_exit_false(void);
static bool test_firing_expression_no_init(void);
static bool test_firing_expression_no_analyze(void);
static bool test_firing_expression_empty(void);
static bool test_firing_expression_undefined_timer_remove(void);
static bool test_firing_expression_undefined_periodic_hold(void);
static bool test_firing_expression_undefined_periodic_release(void);
static bool test_firing_expression_undefined_periodic_remove(void);
static bool test_firing_expression_undefined_on_exit_hold(void);
static bool test_firing_expression_undefined_on_exit_remove(void);
static bool test_firing_expression_only_timer_remove(void);
static bool test_firing_expression_only_periodic_hold(void);
static bool test_firing_expression_only_periodic_release(void);
static bool test_firing_expression_only_periodic_remove(void);
static bool test_firing_expression_only_false(void);
static bool test_firing_expression_exit_timer_remove(void);
static bool test_firing_expression_exit_periodic_hold(void);
static bool test_firing_expression_exit_periodic_release(void);
static bool test_firing_expression_exit_periodic_remove(void);
static bool test_firing_expression_exit_on_exit_hold(void);
static bool test_firing_expression_exit_on_exit_remove(void);
static bool test_firing_expression_exit_false(void);
static bool test_firing_expression_value_no_init(void);
static bool test_firing_expression_value_no_analyze(void);
static bool test_firing_expression_value_empty(void);
static bool test_firing_expression_value_und_timer_remove(void);
static bool test_firing_expression_value_und_periodic_hold(void);
static bool test_firing_expression_value_und_periodic_release(void);
static bool test_firing_expression_value_und_periodic_remove(void);
static bool test_firing_expression_value_und_on_exit_hold(void);
static bool test_firing_expression_value_und_on_exit_remove(void);
static bool test_firing_expression_value_only_timer_remove(void);
static bool test_firing_expression_value_only_periodic_hold(void);
static bool test_firing_expression_value_only_periodic_release(void);
static bool test_firing_expression_value_only_periodic_remove(void);
static bool test_firing_expression_value_only_false(void);
static bool test_firing_expression_value_exit_timer_remove(void);
static bool test_firing_expression_value_exit_periodic_hold(void);
static bool test_firing_expression_value_exit_periodic_release(void);
static bool test_firing_expression_value_exit_periodic_remove(void);
static bool test_firing_expression_value_exit_on_exit_hold(void);
static bool test_firing_expression_value_exit_on_exit_remove(void);
static bool test_firing_expression_value_exit_false(void);
static bool test_firing_reason_no_init(void);
static bool test_firing_reason_no_analyze(void);
static bool test_firing_reason_empty(void);
static bool test_firing_reason_undefined_timer_remove(void);
static bool test_firing_reason_undefined_periodic_hold(void);
static bool test_firing_reason_undefined_periodic_release(void);
static bool test_firing_reason_undefined_periodic_remove(void);
static bool test_firing_reason_undefined_on_exit_hold(void);
static bool test_firing_reason_undefined_on_exit_remove(void);
static bool test_firing_reason_only_timer_remove(void);
static bool test_firing_reason_only_periodic_hold(void);
static bool test_firing_reason_only_periodic_release(void);
static bool test_firing_reason_only_periodic_remove(void);
static bool test_firing_reason_only_false(void);
static bool test_firing_reason_exit_timer_remove(void);
static bool test_firing_reason_exit_periodic_hold(void);
static bool test_firing_reason_exit_periodic_release(void);
static bool test_firing_reason_exit_periodic_remove(void);
static bool test_firing_reason_exit_on_exit_hold(void);
static bool test_firing_reason_exit_on_exit_remove(void);
static bool test_firing_reason_exit_false(void);
static bool test_remove_macro_analyze_policy(void);
static bool test_remove_macro_firing_expression(void);
static bool test_remove_macro_firing_expression_value(void);
static bool test_remove_macro_firing_reason(void);
static bool test_release_macro_analyze_policy(void);
static bool test_release_macro_firing_expression(void);
static bool test_release_macro_firing_expression_value(void);
static bool test_release_macro_firing_reason(void);
static bool test_hold_macro_analyze_policy(void);
static bool test_hold_macro_firing_expression(void);
static bool test_hold_macro_firing_expression_value(void);
static bool test_hold_macro_firing_reason(void);

//global variables
static ClassAdParser parser;
static ClassAdUnParser unparser;
static compat_classad::ClassAd* ad;
static string classad_string;
static MyString reason;

//string constants used for initializing ClassAds
static const char
	*TIMER_REMOVE = 
	"\tTimerRemove = 0\n\t\tPeriodicHold = true\n\t\t"
	"PeriodicRemove = true\n\t\tPeriodicRelease = true\n\t\t"
	"OnExitHold = true\n\t\tOnExitRemove = true\n\t\tJobStatus = 2",
	*PERIODIC_HOLD = 
	"\tPeriodicHold = true\n\t\t"
	"PeriodicRemove = true\n\t\tPeriodicRelease = true\n\t\t"
	"OnExitHold = true\n\t\tOnExitRemove = true\n\t\tJobStatus = 2",
	*PERIODIC_RELEASE = 
	"\tPeriodicHold = false\n\t\t"
	"PeriodicRelease = true\n\t\tPeriodicRemove = true\n\t\t"
	"OnExitHold = true\n\t\tOnExitRemove = true\n\t\tJobStatus = 5",
	*PERIODIC_REMOVE = 
	"\tPeriodicHold = false\n\t\t"
	"PeriodicRelease = false\n\t\tPeriodicRemove= true\n\t\t"
	"OnExitHold = true\n\t\tOnExitRemove = true\n\t\tJobStatus = 2",
	*ON_EXIT_HOLD = 
	"\tPeriodicHold = false\n\t\tPeriodicRemove = false\n\t\t"
	"PeriodicRelease = false\n\t\tOnExitHold = true\n\t\t"
	"OnExitRemove = true\n\t\tExitBySignal = true\n\t\tExitSignal = 123\n\t\t"
	"ExitCode = 321\n\t\tJobStatus = 2",
	*ON_EXIT_REMOVE = 
	"\tPeriodicHold = false\n\t\tPeriodicRemove = false\n\t\t"
	"PeriodicRelease = false\n\t\tOnExitHold = false\n\t\t"
	"OnExitRemove = true\n\t\tExitBySignal = true\n\t\tExitSignal = 123\n\t\t"
	"ExitCode = 321\n\t\tJobStatus = 2",
	*DEFAULT = 
	"[ PeriodicHold = false; PeriodicRemove = false; PeriodicRelease = false; "
	"OnExitHold = false; OnExitRemove = true; CurrentTime = time() ]";

bool OTEST_UserPolicy(void) {
	emit_object("UserPolicy");

	FunctionDriver driver;
	driver.register_function(test_init_null);
	driver.register_function(test_init_empty);
	driver.register_function(test_init_non_empty);
	driver.register_function(test_init_non_empty_miss1);
	driver.register_function(test_init_non_empty_miss2);
	driver.register_function(test_analyze_policy_no_init);
	driver.register_function(test_analyze_policy_invalid_mode);
	driver.register_function(test_analyze_policy_missing_job_status);
	driver.register_function(test_analyze_policy_empty);
	driver.register_function(test_analyze_policy_undefined_timer_remove);
	driver.register_function(test_analyze_policy_undefined_periodic_hold);
	driver.register_function(test_analyze_policy_undefined_periodic_release);
	driver.register_function(test_analyze_policy_undefined_periodic_remove);
	driver.register_function(test_analyze_policy_undefined_on_exit_hold);
	driver.register_function(test_analyze_policy_undefined_on_exit_remove);
	driver.register_function(test_analyze_policy_only_timer_remove);
	driver.register_function(test_analyze_policy_only_periodic_hold);
	driver.register_function(test_analyze_policy_only_periodic_hold_false);
	driver.register_function(test_analyze_policy_only_periodic_release);
	driver.register_function(test_analyze_policy_only_periodic_release_false);
	driver.register_function(test_analyze_policy_only_periodic_remove);
	driver.register_function(test_analyze_policy_only_false);
	driver.register_function(test_analyze_policy_exit_missing);
	driver.register_function(test_analyze_policy_exit_missing_both);
	driver.register_function(test_analyze_policy_exit_missing_signal);
	driver.register_function(test_analyze_policy_exit_missing_code);
	driver.register_function(test_analyze_policy_exit_timer_remove);
	driver.register_function(test_analyze_policy_exit_periodic_hold);
	driver.register_function(test_analyze_policy_exit_periodic_hold_false);
	driver.register_function(test_analyze_policy_exit_periodic_release);
	driver.register_function(test_analyze_policy_exit_periodic_release_false);
	driver.register_function(test_analyze_policy_exit_periodic_remove);
	driver.register_function(test_analyze_policy_exit_on_exit_hold);
	driver.register_function(test_analyze_policy_exit_on_exit_remove);
	driver.register_function(test_analyze_policy_exit_false);
	driver.register_function(test_firing_expression_no_init);
	driver.register_function(test_firing_expression_no_analyze);
	driver.register_function(test_firing_expression_empty);
	driver.register_function(test_firing_expression_undefined_timer_remove);
	driver.register_function(test_firing_expression_undefined_periodic_hold);
	driver.register_function(test_firing_expression_undefined_periodic_release);
	driver.register_function(test_firing_expression_undefined_periodic_remove);
	driver.register_function(test_firing_expression_undefined_on_exit_hold);
	driver.register_function(test_firing_expression_undefined_on_exit_remove);
	driver.register_function(test_firing_expression_only_timer_remove);
	driver.register_function(test_firing_expression_only_periodic_hold);
	driver.register_function(test_firing_expression_only_periodic_release);
	driver.register_function(test_firing_expression_only_periodic_remove);
	driver.register_function(test_firing_expression_only_false);
	driver.register_function(test_firing_expression_exit_timer_remove);
	driver.register_function(test_firing_expression_exit_periodic_hold);
	driver.register_function(test_firing_expression_exit_periodic_release);
	driver.register_function(test_firing_expression_exit_periodic_remove);
	driver.register_function(test_firing_expression_exit_on_exit_hold);
	driver.register_function(test_firing_expression_exit_on_exit_remove);
	driver.register_function(test_firing_expression_exit_false);
	driver.register_function(test_firing_expression_value_no_init);
	driver.register_function(test_firing_expression_value_no_analyze);
	driver.register_function(test_firing_expression_value_empty);
	driver.register_function(test_firing_expression_value_und_timer_remove);
	driver.register_function(test_firing_expression_value_und_periodic_hold);
	driver.register_function(test_firing_expression_value_und_periodic_release);
	driver.register_function(test_firing_expression_value_und_periodic_remove);
	driver.register_function(test_firing_expression_value_und_on_exit_hold);
	driver.register_function(test_firing_expression_value_und_on_exit_remove);
	driver.register_function(test_firing_expression_value_only_timer_remove);
	driver.register_function(test_firing_expression_value_only_periodic_hold);
	driver.register_function(test_firing_expression_value_only_periodic_release);
	driver.register_function(test_firing_expression_value_only_periodic_remove);
	driver.register_function(test_firing_expression_value_only_false);
	driver.register_function(test_firing_expression_value_exit_timer_remove);
	driver.register_function(test_firing_expression_value_exit_periodic_hold);
	driver.register_function(test_firing_expression_value_exit_periodic_release);
	driver.register_function(test_firing_expression_value_exit_periodic_remove);
	driver.register_function(test_firing_expression_value_exit_on_exit_hold);
	driver.register_function(test_firing_expression_value_exit_on_exit_remove);
	driver.register_function(test_firing_expression_value_exit_false);
	driver.register_function(test_firing_reason_no_init);
	driver.register_function(test_firing_reason_no_analyze);
	driver.register_function(test_firing_reason_empty);
	driver.register_function(test_firing_reason_undefined_timer_remove);
	driver.register_function(test_firing_reason_undefined_periodic_hold);
	driver.register_function(test_firing_reason_undefined_periodic_release);
	driver.register_function(test_firing_reason_undefined_periodic_remove);
	driver.register_function(test_firing_reason_undefined_on_exit_hold);
	driver.register_function(test_firing_reason_undefined_on_exit_remove);
	driver.register_function(test_firing_reason_only_timer_remove);
	driver.register_function(test_firing_reason_only_periodic_hold);
	driver.register_function(test_firing_reason_only_periodic_release);
	driver.register_function(test_firing_reason_only_periodic_remove);
	driver.register_function(test_firing_reason_only_false);
	driver.register_function(test_firing_reason_exit_timer_remove);
	driver.register_function(test_firing_reason_exit_periodic_hold);
	driver.register_function(test_firing_reason_exit_periodic_release);
	driver.register_function(test_firing_reason_exit_periodic_remove);
	driver.register_function(test_firing_reason_exit_on_exit_hold);
	driver.register_function(test_firing_reason_exit_on_exit_remove);
	driver.register_function(test_firing_reason_exit_false);
	driver.register_function(test_remove_macro_analyze_policy);
	driver.register_function(test_remove_macro_firing_expression);
	driver.register_function(test_remove_macro_firing_expression_value);
	driver.register_function(test_remove_macro_firing_reason);
	driver.register_function(test_release_macro_analyze_policy);
	driver.register_function(test_release_macro_firing_expression);
	driver.register_function(test_release_macro_firing_expression_value);
	driver.register_function(test_release_macro_firing_reason);
	driver.register_function(test_hold_macro_analyze_policy);
	driver.register_function(test_hold_macro_firing_expression);
	driver.register_function(test_hold_macro_firing_expression_value);
	driver.register_function(test_hold_macro_firing_reason);
	
	return driver.do_all_functions();
}

static bool test_init_null() {
	emit_test("Test Init() when passed a NULL ClassAd pointer.");
	emit_comment("There is no way to check if this test fails, so as long as we"
		" don't segfault it passes.");
	emit_problem("By inspection, AnalyzePolicy() code correctly ASSERTS for a "
		"ClassAd that is NULL, but we can't verify that in the current unit "
		"test framework.");
	emit_comment("See ticket #1570.");
	emit_input_header();
	emit_param("ClassAd", "NULL");
	//UserPolicy policy;
	//policy.Init(NULL);
	PASS;
}

static bool test_init_empty() {
	emit_test("Test that Init() sets the required attributes to the default "
		"values when passed an empty ClassAd.");
	emit_input_header();
	emit_param("ClassAd", "");
	emit_output_expected_header();
	emit_param("ClassAd", "%s", DEFAULT);
	ad = new compat_classad::ClassAd();
	UserPolicy policy;
	policy.Init(ad);	//use default attributes
	unparser.Unparse(classad_string, ad);
	emit_output_actual_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	if(!user_policy_ad_checker(ad, false, false, false, false, true)) {
		CLEANUP;
		FAIL;
	}
	CLEANUP;
	PASS;
}

static bool test_init_non_empty() {
	emit_test("Test that Init() doesn't change any of the required attributes "
		"when passed a ClassAd that already contains the attributes needed for"
		" UserPolicy.");
	const char* expect = "[ PeriodicHold = true; PeriodicRemove = true; "
		"PeriodicRelease = true; OnExitHold = true; OnExitRemove = false; "
		"CurrentTime = time() ]";
	emit_input_header();
	emit_param("ClassAd", "%s", expect);
	emit_output_expected_header();
	emit_param("ClassAd", "%s", expect);
	ad = new compat_classad::ClassAd();
	ad->initFromString("PeriodicHold = true\nPeriodicRemove = true\n"
		"PeriodicRelease = true\nOnExitHold = true\nOnExitRemove = false");
	UserPolicy policy;
	policy.Init(ad);
	unparser.Unparse(classad_string, ad);
	emit_output_actual_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	if(!user_policy_ad_checker(ad, true, true, true, true, false)) {	
		CLEANUP;
		FAIL;
	}
	CLEANUP;
	PASS;
}

static bool test_init_non_empty_miss1() {
	emit_test("Test that Init() sets the required attributes to the default "
		"values when passed a ClassAd that already contains the first three "
		"of the attributes needed for UserPolicy.");
	const char* input = 
		"[ TimerRemove = 0; PeriodicHold = true; PeriodicRemove = true ]";
	const char* expect = 
		"[ TimerRemove = 0; PeriodicHold = true; PeriodicRemove = true; "
		"PeriodicRelease = false; OnExitHold = false; OnExitRemove = true; "
		"CurrentTime = time() ]";
	emit_input_header();
	emit_param("ClassAd", "%s", input);
	emit_output_expected_header();
	emit_param("ClassAd", "%s", expect);
	ad = new compat_classad::ClassAd();
	ad->initFromString("TimerRemove = 0\nPeriodicHold = true\n"
		"PeriodicRemove = true");
	UserPolicy policy;
	policy.Init(ad);
	unparser.Unparse(classad_string, ad);
	emit_output_actual_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	if(!user_policy_ad_checker(ad, false, true, true, false, false, true)) {
		CLEANUP;
		FAIL;
	}
	CLEANUP;
	PASS;
}

static bool test_init_non_empty_miss2() {
	emit_test("Test that Init() sets the required attributes to the default "
		"values when passed a ClassAd that already contains the last three "
		"of the attributes needed for UserPolicy.");
	const char* input = "[ PeriodicRelease = true; OnExitHold = true; "
		"OnExitRemove = true ]";
	const char* expect = "[ PeriodicHold = false; PeriodicRemove = false; "
		"PeriodicRelease = true; OnExitHold = true; OnExitRemove = true; "
		"CurrentTime = time() ]";
	emit_input_header();
	emit_param("ClassAd", "%s", input);
	emit_output_expected_header();
	emit_param("ClassAd", "%s", expect);
	ad = new compat_classad::ClassAd();
	ad->initFromString("PeriodicRelease = true\nOnExitHold = true\n"
		"OnExitRemove = true");
	UserPolicy policy;
	policy.Init(ad);
	unparser.Unparse(classad_string, ad);
	emit_output_actual_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	if(!user_policy_ad_checker(ad, false, false, true, true, true)) {
		CLEANUP;
		FAIL;
	}
	CLEANUP;
	PASS;
}

static bool test_analyze_policy_no_init() {
	emit_test("Test AnalyzePolicy() without calling Init() on the UserPolicy "
		"object first.");
	emit_problem("By inspection, AnalyzePolicy() code correctly EXCEPTS for a "
		"ClassAd that is NULL (didn't call Init()), but we can't verify that in"
		" the current unit test framework.");
/*
	emit_input_header();
	emit_param("ClassAd", "NULL");
	emit_param("Mode", "PERIODIC_ONLY");
	UserPolicy policy;
	policy.AnalyzePolicy(PERIODIC_ONLY);
*/
	PASS;
}

static bool test_analyze_policy_invalid_mode() {
	emit_test("Test AnalyzePolicy() with an invalid mode.");
	emit_problem("By inspection, AnalyzePolicy() code correctly EXCEPTS for an"
		" invalid mode, but we can't verify that in the current unit test "
		"framework.");
/*
	emit_input_header();
	emit_param("ClassAd", "%s", DEFAULT);
	emit_param("Mode", "%d", -1);
	ad = new compat_classad::ClassAd();
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(-1);
	CLEANUP;
*/
	PASS;
}

static bool test_analyze_policy_missing_job_status() {
	emit_test("Test that AnalyzePolicy() returns UNDEFINED_EVAL when used with "
		"a ClassAd that is missing a job status.");
	emit_input_header();
	emit_param("ClassAd", "%s", DEFAULT);
	emit_param("Mode", "PERIODIC_ONLY");
	emit_output_expected_header();
	emit_retval("%d", UNDEFINED_EVAL);
	ad = new compat_classad::ClassAd();
	UserPolicy policy;
	policy.Init(ad);	//use default values
	int ret_val = policy.AnalyzePolicy(PERIODIC_ONLY);
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != UNDEFINED_EVAL) {
		FAIL;
	}
	PASS;
}

static bool test_analyze_policy_empty() {
	emit_test("Test that AnalyzePolicy() returns UNDEFINED_EVAL when used with "
		"a ClassAd that is empty.");
	emit_input_header();
	emit_param("ClassAd", "");
	emit_param("Mode", "PERIODIC_ONLY");
	emit_output_expected_header();
	emit_retval("%d", UNDEFINED_EVAL);
	ad = new compat_classad::ClassAd();
	UserPolicy policy;
	policy.Init(ad);	//use default values
	ad->Delete(ATTR_PERIODIC_HOLD_CHECK);
	ad->Delete(ATTR_PERIODIC_RELEASE_CHECK);
	ad->Delete(ATTR_PERIODIC_REMOVE_CHECK);
	ad->Delete(ATTR_ON_EXIT_HOLD_CHECK);
	ad->Delete(ATTR_ON_EXIT_REMOVE_CHECK);
	ad->Delete(ATTR_CURRENT_TIME);
	int ret_val = policy.AnalyzePolicy(PERIODIC_ONLY);
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != UNDEFINED_EVAL) {
		FAIL;
	}
	PASS;
}

static bool test_analyze_policy_undefined_timer_remove() {
	emit_test("Test that AnalyzePolicy() returns UNDEFINED_EVAL when used with "
		"a ClassAd that has TimerRemove evaluate to undefined.");
	emit_comment("See ticket #1571.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(TIMER_REMOVE);
	insert_into_ad(ad, ATTR_TIMER_REMOVE_CHECK, "DoesNotExist");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_param("Mode", "PERIODIC_ONLY");
	emit_output_expected_header();
	emit_retval("%d", UNDEFINED_EVAL);
	UserPolicy policy;
	policy.Init(ad);
	int ret_val = policy.AnalyzePolicy(PERIODIC_ONLY);
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != UNDEFINED_EVAL) {
		FAIL;
	}
	PASS;
}

static bool test_analyze_policy_undefined_periodic_hold() {
	emit_test("Test that AnalyzePolicy() returns UNDEFINED_EVAL when used with "
		"a ClassAd that has PeriodicHold evaluate to undefined.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_HOLD);
	insert_into_ad(ad, ATTR_PERIODIC_HOLD_CHECK, "DoesNotExist");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_param("Mode", "PERIODIC_ONLY");
	emit_output_expected_header();
	emit_retval("%d", UNDEFINED_EVAL);
	UserPolicy policy;
	policy.Init(ad);
	int ret_val = policy.AnalyzePolicy(PERIODIC_ONLY);
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != UNDEFINED_EVAL) {
		FAIL;
	}
	PASS;
}

static bool test_analyze_policy_undefined_periodic_release() {
	emit_test("Test that AnalyzePolicy() returns UNDEFINED_EVAL when used with "
		"a ClassAd that has PeriodicRelease evaluate to undefined.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_RELEASE);
	insert_into_ad(ad, ATTR_PERIODIC_RELEASE_CHECK, "DoesNotExist");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_param("Mode", "PERIODIC_ONLY");
	emit_output_expected_header();
	emit_retval("%d", UNDEFINED_EVAL);
	UserPolicy policy;
	policy.Init(ad);
	int ret_val = policy.AnalyzePolicy(PERIODIC_ONLY);
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != UNDEFINED_EVAL) {
		FAIL;
	}
	PASS;
}

static bool test_analyze_policy_undefined_periodic_remove() {
	emit_test("Test that AnalyzePolicy() returns UNDEFINED_EVAL when used with "
		"a ClassAd that has PeriodicRemove evaluate to undefined.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_REMOVE);
	insert_into_ad(ad, ATTR_PERIODIC_REMOVE_CHECK, "DoesNotExist");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_param("Mode", "PERIODIC_ONLY");
	emit_output_expected_header();
	emit_retval("%d", UNDEFINED_EVAL);
	UserPolicy policy;
	policy.Init(ad);
	int ret_val = policy.AnalyzePolicy(PERIODIC_ONLY);
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != UNDEFINED_EVAL) {
		FAIL;
	}
	PASS;
}

static bool test_analyze_policy_undefined_on_exit_hold() {
	emit_test("Test that AnalyzePolicy() returns UNDEFINED_EVAL when used with "
		"a ClassAd that has OnExitHold evaluate to undefined.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(ON_EXIT_HOLD);
	insert_into_ad(ad, ATTR_ON_EXIT_HOLD_CHECK, "DoesNotExist");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_param("Mode", "PERIODIC_THEN_EXIT");
	emit_output_expected_header();
	emit_retval("%d", UNDEFINED_EVAL);
	UserPolicy policy;
	policy.Init(ad);
	int ret_val = policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != UNDEFINED_EVAL) {
		FAIL;
	}
	PASS;
}

static bool test_analyze_policy_undefined_on_exit_remove() {
	emit_test("Test that AnalyzePolicy() returns UNDEFINED_EVAL when used with "
		"a ClassAd that has OnExitRemove evaluate to undefined.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(ON_EXIT_REMOVE);
	insert_into_ad(ad, ATTR_ON_EXIT_REMOVE_CHECK, "DoesNotExist");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_param("Mode", "PERIODIC_THEN_EXIT");
	emit_output_expected_header();
	emit_retval("%d", UNDEFINED_EVAL);
	UserPolicy policy;
	policy.Init(ad);
	int ret_val = policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != UNDEFINED_EVAL) {
		FAIL;
	}
	PASS;
}

static bool test_analyze_policy_only_timer_remove() {
	emit_test("Test that AnalyzePolicy() returns REMOVE_FROM_QUEUE when used "
		"with the PERIODIC_ONLY mode and a ClassAd that has TimerRemove "
		"evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(TIMER_REMOVE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_param("Mode", "PERIODIC_ONLY");
	emit_output_expected_header();
	emit_retval("%d", REMOVE_FROM_QUEUE);
	UserPolicy policy;
	policy.Init(ad);
	int ret_val = policy.AnalyzePolicy(PERIODIC_ONLY);
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != REMOVE_FROM_QUEUE) {
		FAIL;
	}
	PASS;
}

static bool test_analyze_policy_only_periodic_hold() {
	emit_test("Test that AnalyzePolicy() returns HOLD_IN_QUEUE when used with "
		"the PERIODIC_ONLY mode and a ClassAd that has PeriodicHold evaluate "
		"to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_HOLD);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_param("Mode", "PERIODIC_ONLY");
	emit_output_expected_header();
	emit_retval("%d", HOLD_IN_QUEUE);
	UserPolicy policy;
	policy.Init(ad);
	int ret_val = policy.AnalyzePolicy(PERIODIC_ONLY);
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != HOLD_IN_QUEUE) {
		FAIL;
	}
	PASS;
}

static bool test_analyze_policy_only_periodic_hold_false() {
	emit_test("Test that AnalyzePolicy() doesn't return HOLD_IN_QUEUE when used"
		" with the PERIODIC_ONLY mode and a ClassAd that has PeriodicHold "
		"evaluate to false.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_RELEASE);
	insert_into_ad(ad, ATTR_JOB_STATUS, "2");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_param("Mode", "PERIODIC_ONLY");
	emit_output_expected_header();
	emit_retval("Not %d", HOLD_IN_QUEUE);
	UserPolicy policy;
	policy.Init(ad);
	int ret_val = policy.AnalyzePolicy(PERIODIC_ONLY);
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val == HOLD_IN_QUEUE) {
		FAIL;
	}
	PASS;
}

//This test now fails?
static bool test_analyze_policy_only_periodic_release() {
	emit_test("Test that AnalyzePolicy() returns RELEASE_FROM_HOLD when used"
		" with the PERIODIC_ONLY mode and a ClassAd that has PeriodicRelease "
		"evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_RELEASE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_param("Mode", "PERIODIC_ONLY");
	emit_output_expected_header();
	emit_retval("%d", RELEASE_FROM_HOLD);
	UserPolicy policy;
	policy.Init(ad);
	int ret_val = policy.AnalyzePolicy(PERIODIC_ONLY);
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != RELEASE_FROM_HOLD) {
		FAIL;
	}
	PASS;
}

static bool test_analyze_policy_only_periodic_release_false() {
	emit_test("Test that AnalyzePolicy() doesn't return RELEASE_FROM_HOLD when "
		"used with the PERIODIC_ONLY mode and a ClassAd that has "
		"PeriodicRelease evaluate to false.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_REMOVE);
	insert_into_ad(ad, ATTR_JOB_STATUS, "5");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_param("Mode", "PERIODIC_ONLY");
	emit_output_expected_header();
	emit_retval("Not %d", RELEASE_FROM_HOLD);
	UserPolicy policy;
	policy.Init(ad);
	int ret_val = policy.AnalyzePolicy(PERIODIC_ONLY);
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val == RELEASE_FROM_HOLD) {
		FAIL;
	}
	PASS;
}

static bool test_analyze_policy_only_periodic_remove() {
	emit_test("Test that AnalyzePolicy() returns REMOVE_FROM_QUEUE when used"
		" with the PERIODIC_ONLY mode and a ClassAd that has PeriodicRemove "
		"evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_REMOVE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_param("Mode", "PERIODIC_ONLY");
	emit_output_expected_header();
	emit_retval("%d", REMOVE_FROM_QUEUE);
	UserPolicy policy;
	policy.Init(ad);
	int ret_val = policy.AnalyzePolicy(PERIODIC_ONLY);
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != REMOVE_FROM_QUEUE) {
		FAIL;
	}
	PASS;
}

static bool test_analyze_policy_only_false() {
	emit_test("Test that AnalyzePolicy() returns STAYS_IN_QUEUE when used with "
		"the PERIODIC_ONLY mode and a ClassAd that has all the checked "
		"attributes evaluate to false.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(ON_EXIT_HOLD);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_param("Mode", "PERIODIC_ONLY");
	emit_output_expected_header();
	emit_retval("%d", STAYS_IN_QUEUE);
	UserPolicy policy;
	policy.Init(ad);
	int ret_val = policy.AnalyzePolicy(PERIODIC_ONLY);
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != STAYS_IN_QUEUE) {
		FAIL;
	}
	PASS;
}

static bool test_analyze_policy_exit_missing() {
	emit_test("Test AnalyzePolicy() when used with the PERIODIC_THEN_EXIT mode,"
		" but with a ClassAd that is missing the ExitBySignal attribute.");
	emit_problem("By inspection, AnalyzePolicy() code correctly EXCEPTS for a "
		"ClassAd this is missing the ExitBySignal attribute, but we can't "
		"verify that in the current unit test framework.");
/*	
	ad = new compat_classad::ClassAd();
	ad->initFromString(ON_EXIT_REMOVE);
	ad->Delete(ATTR_ON_EXIT_BY_SIGNAL);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_param("Mode", "PERIODIC_THEN_EXIT");
	emit_output_expected_header();
	emit_retval("%d", UNDEFINED_EVAL);
	UserPolicy policy;
	policy.Init(ad);
	int ret_val = policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != UNDEFINED_EVAL) {
		FAIL;
	}
*/
	PASS;
}

static bool test_analyze_policy_exit_missing_both() {
	emit_test("Test AnalyzePolicy() when used with the PERIODIC_THEN_EXIT mode,"
		" but with a ClassAd that is missing the ExitSignal and ExitCode "
		"attributes.");
	emit_problem("By inspection, AnalyzePolicy() code correctly EXCEPTS for a "
		"ClassAd this is missing both the ExitSignal and ExitCode attributes, "
		"but we can't verify that in the current unit test framework.");
/*
	ad = new compat_classad::ClassAd();
	ad->initFromString(ON_EXIT_REMOVE);
	ad->Delete(ATTR_ON_EXIT_SIGNAL);
	ad->Delete(ATTR_ON_EXIT_CODE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_param("Mode", "PERIODIC_THEN_EXIT");
	emit_output_expected_header();
	emit_retval("%d", UNDEFINED_EVAL);
	UserPolicy policy;
	policy.Init(ad);
	int ret_val = policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != UNDEFINED_EVAL) {
		FAIL;
	}
*/
	PASS;
}

static bool test_analyze_policy_exit_missing_signal() {
	emit_test("Test that AnalyzePolicy() returns STAYS_IN_QUEUE when used with "
		"the PERIODIC_THEN_EXIT mode and a ClassAd that has OnExitBySignal and "
		"OnExitCode defined, but is missing the OnExitSignal attribute.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(ON_EXIT_REMOVE);
	ad->Delete(ATTR_ON_EXIT_SIGNAL);
	insert_into_ad(ad, ATTR_ON_EXIT_REMOVE_CHECK, "false");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_param("Mode", "PERIODIC_THEN_EXIT");
	emit_output_expected_header();
	emit_retval("%d", STAYS_IN_QUEUE);
	UserPolicy policy;
	policy.Init(ad);
	int ret_val = policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	classad_string.clear();
	CLEANUP;
	if(ret_val != STAYS_IN_QUEUE) {
		FAIL;
	}
	PASS;
}

static bool test_analyze_policy_exit_missing_code() {
	emit_test("Test that AnalyzePolicy() returns STAYS_IN_QUEUE when used with "
		"the PERIODIC_THEN_EXIT mode and a ClassAd that has ExitBySignal and "
		"ExitSignal defined, but is missing the ExitCode attribute.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(ON_EXIT_REMOVE);
	ad->Delete(ATTR_ON_EXIT_CODE);
	insert_into_ad(ad, ATTR_ON_EXIT_REMOVE_CHECK, "false");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_param("Mode", "PERIODIC_THEN_EXIT");
	emit_output_expected_header();
	emit_retval("%d", STAYS_IN_QUEUE);
	UserPolicy policy;
	policy.Init(ad);
	int ret_val = policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	classad_string.clear();
	CLEANUP;
	if(ret_val != STAYS_IN_QUEUE) {
		FAIL;
	}
	PASS;
}

static bool test_analyze_policy_exit_timer_remove() {
	emit_test("Test that AnalyzePolicy() returns REMOVE_FROM_QUEUE when used "
		"with the PERIODIC_THEN_EXIT mode and a ClassAd that has TimerRemove "
		"evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(TIMER_REMOVE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_param("Mode", "PERIODIC_THEN_EXIT");
	emit_output_expected_header();
	emit_retval("%d", REMOVE_FROM_QUEUE);
	UserPolicy policy;
	policy.Init(ad);
	int ret_val = policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != REMOVE_FROM_QUEUE) {
		FAIL;
	}
	PASS;
}

static bool test_analyze_policy_exit_periodic_hold() {
	emit_test("Test that AnalyzePolicy() returns HOLD_IN_QUEUE when used with "
		"the PERIODIC_THEN_EXIT mode and a ClassAd that has PeriodicHold "
		"evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_HOLD);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_param("Mode", "PERIODIC_THEN_EXIT");
	emit_output_expected_header();
	emit_retval("%d", HOLD_IN_QUEUE);
	UserPolicy policy;
	policy.Init(ad);
	int ret_val = policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != HOLD_IN_QUEUE) {
		FAIL;
	}
	PASS;
}

static bool test_analyze_policy_exit_periodic_hold_false() {
	emit_test("Test that AnalyzePolicy() doesn't return HOLD_IN_QUEUE when used"
		" with the PERIODIC_THEN_EXIT mode and a ClassAd that has PeriodicHold "
		"evaluate to false.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_RELEASE);
	insert_into_ad(ad, ATTR_JOB_STATUS, "2");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_param("Mode", "PERIODIC_THEN_EXIT");
	emit_output_expected_header();
	emit_retval("Not %d", HOLD_IN_QUEUE);
	UserPolicy policy;
	policy.Init(ad);
	int ret_val = policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val == HOLD_IN_QUEUE) {
		FAIL;
	}
	PASS;
}

static bool test_analyze_policy_exit_periodic_release() {
	emit_test("Test that AnalyzePolicy() returns RELEASE_FROM_HOLD when used"
		" with the PERIODIC_THEN_EXIT mode and a ClassAd that has "
		"PeriodicRelease evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_RELEASE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_param("Mode", "PERIODIC_THEN_EXIT");
	emit_output_expected_header();
	emit_retval("%d", RELEASE_FROM_HOLD);
	UserPolicy policy;
	policy.Init(ad);
	int ret_val = policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != RELEASE_FROM_HOLD) {
		FAIL;
	}
	PASS;
}

static bool test_analyze_policy_exit_periodic_release_false() {
	emit_test("Test that AnalyzePolicy() doesn't return RELEASE_FROM_HOLD when "
		"used with the PERIODIC_THEN_EXIT mode and a ClassAd that has "
		"PeriodicRelease evaluate to false.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_REMOVE);
	insert_into_ad(ad, ATTR_JOB_STATUS, "5");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_param("Mode", "PERIODIC_THEN_EXIT");
	emit_output_expected_header();
	emit_retval("Not %d", RELEASE_FROM_HOLD);
	UserPolicy policy;
	policy.Init(ad);
	int ret_val = policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val == RELEASE_FROM_HOLD) {
		FAIL;
	}
	PASS;
}

static bool test_analyze_policy_exit_periodic_remove() {
	emit_test("Test that AnalyzePolicy() returns REMOVE_FROM_QUEUE when used"
		" with the PERIODIC_THEN_EXIT mode and a ClassAd that has "
		"PeriodicRemove evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_REMOVE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_param("Mode", "PERIODIC_THEN_EXIT");
	emit_output_expected_header();
	emit_retval("%d", REMOVE_FROM_QUEUE);
	UserPolicy policy;
	policy.Init(ad);
	int ret_val = policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != REMOVE_FROM_QUEUE) {
		FAIL;
	}
	PASS;
}

static bool test_analyze_policy_exit_on_exit_hold() {
	emit_test("Test that AnalyzePolicy() returns HOLD_IN_QUEUE when used"
		" with the PERIODIC_THEN_EXIT mode and a ClassAd that has "
		"OnExitRemove evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(ON_EXIT_HOLD);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_param("Mode", "PERIODIC_THEN_EXIT");
	emit_output_expected_header();
	emit_retval("%d", HOLD_IN_QUEUE);
	UserPolicy policy;
	policy.Init(ad);
	int ret_val = policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != HOLD_IN_QUEUE) {
		FAIL;
	}
	PASS;
}

static bool test_analyze_policy_exit_on_exit_remove() {
	emit_test("Test that AnalyzePolicy() returns REMOVE_FROM_QUEUE when used"
		" with the PERIODIC_THEN_EXIT mode and a ClassAd that has "
		"OnExitRemove evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(ON_EXIT_REMOVE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_param("Mode", "PERIODIC_THEN_EXIT");
	emit_output_expected_header();
	emit_retval("%d", REMOVE_FROM_QUEUE);
	UserPolicy policy;
	policy.Init(ad);
	int ret_val = policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != REMOVE_FROM_QUEUE) {
		FAIL;
	}
	PASS;
}

static bool test_analyze_policy_exit_false() {
	emit_test("Test that AnalyzePolicy() returns STAYS_IN_QUEUE when used with "
		"the PERIODIC_THEN_EXIT mode and a ClassAd that has all the checked "
		"attributes evaluate to false.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(ON_EXIT_REMOVE);
	insert_into_ad(ad, ATTR_ON_EXIT_REMOVE_CHECK, "false");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_param("Mode", "PERIODIC_THEN_EXIT");
	emit_output_expected_header();
	emit_retval("%d", STAYS_IN_QUEUE);
	UserPolicy policy;
	policy.Init(ad);
	int ret_val = policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != STAYS_IN_QUEUE) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_no_init() {
	emit_test("Test that FiringExpression() returns NULL when called before "
		"calling Init().");
	emit_input_header();
	emit_param("ClassAd", "NULL");
	emit_output_expected_header();
	emit_param("Returns NULL", "TRUE");
	UserPolicy policy;
	const char* ret_val = policy.FiringExpression();
	emit_output_actual_header();
	emit_param("Returns NULL", "%s", tfstr(ret_val == NULL));
	if(ret_val != NULL) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_no_analyze() {
	emit_test("Test that FiringExpression() returns NULL when called before "
		"calling AnalyzePolicy().");
	ad = new compat_classad::ClassAd();
	UserPolicy policy;
	policy.Init(ad);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_param("Returns NULL", "TRUE");
	const char* ret_val = policy.FiringExpression();
	emit_output_actual_header();
	emit_param("Returns NULL", "%s", tfstr(ret_val == NULL));
	CLEANUP;
	if(ret_val != NULL) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_empty() {
	emit_test("Test that FiringExpression() returns NULL after a call to "
		"AnalyzePolicy() with an empty ClassAd.");
	emit_input_header();
	emit_param("ClassAd", "");
	emit_output_expected_header();
	emit_param("Returns NULL", "TRUE");
	ad = new compat_classad::ClassAd();
	UserPolicy policy;
	policy.Init(ad);	//use default attributes
	ad->Delete(ATTR_PERIODIC_HOLD_CHECK);
	ad->Delete(ATTR_PERIODIC_RELEASE_CHECK);
	ad->Delete(ATTR_PERIODIC_REMOVE_CHECK);
	ad->Delete(ATTR_ON_EXIT_HOLD_CHECK);
	ad->Delete(ATTR_ON_EXIT_REMOVE_CHECK);
	ad->Delete(ATTR_CURRENT_TIME);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	const char* ret_val = policy.FiringExpression();
	emit_output_actual_header();
	emit_param("Returns NULL", "%s", tfstr(ret_val == NULL));
	CLEANUP;
	if(ret_val != NULL) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_undefined_timer_remove() {
	emit_test("Test that FiringExpression() returns TimerRemove after a call to"
		" AnalyzePolicy() with a ClassAd that has TimerRemove evaluate to "
		"undefined.");
	emit_comment("See ticket #1572.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(TIMER_REMOVE);
	insert_into_ad(ad, ATTR_TIMER_REMOVE_CHECK, "DoesNotExist");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%s", ATTR_TIMER_REMOVE_CHECK);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	const char* ret_val = policy.FiringExpression();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, ATTR_TIMER_REMOVE_CHECK) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_undefined_periodic_hold() {
	emit_test("Test that FiringExpression() returns PeriodicHold after a call "
		"to AnalyzePolicy() with a ClassAd that has PeriodicHold evaluate to "
		"undefined.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_HOLD);
	insert_into_ad(ad, ATTR_PERIODIC_HOLD_CHECK, "DoesNotExist");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%s", ATTR_PERIODIC_HOLD_CHECK);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	const char* ret_val = policy.FiringExpression();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, ATTR_PERIODIC_HOLD_CHECK) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_undefined_periodic_release() {
	emit_test("Test that FiringExpression() returns PeriodicRelease after a "
		"call to AnalyzePolicy() with a ClassAd that has PeriodicRelase "
		"evaluate to undefined.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_RELEASE);
	insert_into_ad(ad, ATTR_PERIODIC_RELEASE_CHECK, "DoesNotExist");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%s", ATTR_PERIODIC_RELEASE_CHECK);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	const char* ret_val = policy.FiringExpression();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, ATTR_PERIODIC_RELEASE_CHECK) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_undefined_periodic_remove() {
	emit_test("Test that FiringExpression() returns PeriodicRemove after a "
		"call to AnalyzePolicy() with a ClassAd that has PeriodicRemove "
		"evaluate to undefined.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_REMOVE);
	insert_into_ad(ad, ATTR_PERIODIC_REMOVE_CHECK, "DoesNotExist");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%s", ATTR_PERIODIC_REMOVE_CHECK);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	const char* ret_val = policy.FiringExpression();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, ATTR_PERIODIC_REMOVE_CHECK) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_undefined_on_exit_hold() {
	emit_test("Test that FiringExpression() returns OnExitHold after a "
		"call to AnalyzePolicy() with a ClassAd that has OnExitHold "
		"evaluate to undefined.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(ON_EXIT_HOLD);
	insert_into_ad(ad, ATTR_ON_EXIT_HOLD_CHECK, "DoesNotExist");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%s", ATTR_ON_EXIT_HOLD_CHECK);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	const char* ret_val = policy.FiringExpression();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, ATTR_ON_EXIT_HOLD_CHECK) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_undefined_on_exit_remove() {
	emit_test("Test that FiringExpression() returns OnExitRemove after a "
		"call to AnalyzePolicy() with a ClassAd that has OnExitRemove "
		"evaluate to undefined.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(ON_EXIT_REMOVE);
	insert_into_ad(ad, ATTR_ON_EXIT_REMOVE_CHECK, "DoesNotExist");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%s", ATTR_ON_EXIT_REMOVE_CHECK);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	const char* ret_val = policy.FiringExpression();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, ATTR_ON_EXIT_REMOVE_CHECK) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_only_timer_remove() {
	emit_test("Test that FiringExpression() returns TimerRemove after a call to"
		" AnalyzePolicy() with the PERIODIC_ONLY mode and a ClassAd that has "
		"TimerRemove evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(TIMER_REMOVE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%s", ATTR_TIMER_REMOVE_CHECK);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	const char* ret_val = policy.FiringExpression();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, ATTR_TIMER_REMOVE_CHECK) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_only_periodic_hold() {
	emit_test("Test that FiringExpression() returns PeriodicHold after a call "
		"to AnalyzePolicy() with the PERIODIC_ONLY mode and a ClassAd that has "
		"PeriodicHold evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_HOLD);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%s", ATTR_PERIODIC_HOLD_CHECK);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	const char* ret_val = policy.FiringExpression();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, ATTR_PERIODIC_HOLD_CHECK) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_only_periodic_release() {
	emit_test("Test that FiringExpression() returns PeriodicRelease after a "
		"call to AnalyzePolicy() with the PERIODIC_ONLY mode and a ClassAd that"
		" has PeriodicRelease evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_RELEASE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%s", ATTR_PERIODIC_RELEASE_CHECK);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	const char* ret_val = policy.FiringExpression();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, ATTR_PERIODIC_RELEASE_CHECK) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_only_periodic_remove() {
	emit_test("Test that FiringExpression() returns PeriodicRemove after a "
		"call to AnalyzePolicy() with the PERIODIC_ONLY mode and a ClassAd that"
		" has PeriodicRemove evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_REMOVE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%s", ATTR_PERIODIC_REMOVE_CHECK);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	const char* ret_val = policy.FiringExpression();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, ATTR_PERIODIC_REMOVE_CHECK) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_only_false() {
	emit_test("Test that FiringExpression() returns NULL after a call to "
		"AnalyzePolicy() with the PERIODIC_ONLY mode and a ClassAd that has all"
		" the checked attributes evaluate to false.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(ON_EXIT_REMOVE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_param("Returns NULL", "TRUE");
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	const char* ret_val = policy.FiringExpression();
	emit_output_actual_header();
	emit_param("Returns NULL", "%s", tfstr(ret_val == NULL));
	CLEANUP;
	if(ret_val != NULL) {
		FAIL;
	}
	PASS;
}


static bool test_firing_expression_exit_timer_remove() {
	emit_test("Test that FiringExpression() returns TimerRemove after a call to"
		" AnalyzePolicy() with the PERIODIC_THEN_EXIT mode and a ClassAd that "
		"has TimerRemove evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(TIMER_REMOVE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%s", ATTR_TIMER_REMOVE_CHECK);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	const char* ret_val = policy.FiringExpression();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, ATTR_TIMER_REMOVE_CHECK) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_exit_periodic_hold() {
	emit_test("Test that FiringExpression() returns PeriodicHold after a call "
		"to AnalyzePolicy() with the PERIODIC_THEN_EXIT mode and a ClassAd that"
		" has PeriodicHold evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_HOLD);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%s", ATTR_PERIODIC_HOLD_CHECK);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	const char* ret_val = policy.FiringExpression();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, ATTR_PERIODIC_HOLD_CHECK) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_exit_periodic_release() {
	emit_test("Test that FiringExpression() returns PeriodicRelease after a "
		"call to AnalyzePolicy() with the PERIODIC_THEN_EXIT mode and a ClassAd"
		" that has PeriodicRelease evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_RELEASE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%s", ATTR_PERIODIC_RELEASE_CHECK);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	const char* ret_val = policy.FiringExpression();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, ATTR_PERIODIC_RELEASE_CHECK) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_exit_periodic_remove() {
	emit_test("Test that FiringExpression() returns PeriodicRemove after a "
		"call to AnalyzePolicy() with the PERIODIC_THEN_EXIT mode and a ClassAd that"
		" has PeriodicRemove evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_REMOVE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%s", ATTR_PERIODIC_REMOVE_CHECK);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	const char* ret_val = policy.FiringExpression();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, ATTR_PERIODIC_REMOVE_CHECK) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_exit_on_exit_hold() {
	emit_test("Test that FiringExpression() returns OnExitHold after a "
		"call to AnalyzePolicy() with the PERIODIC_THEN_EXIT mode and a ClassAd that"
		" has OnExitHold evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(ON_EXIT_HOLD);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%s", ATTR_ON_EXIT_HOLD_CHECK);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	const char* ret_val = policy.FiringExpression();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, ATTR_ON_EXIT_HOLD_CHECK) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_exit_on_exit_remove() {
	emit_test("Test that FiringExpression() returns OnExitRemove after a "
		"call to AnalyzePolicy() with the PERIODIC_THEN_EXIT mode and a ClassAd that"
		" has OnExitRemove evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(ON_EXIT_REMOVE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%s", ATTR_ON_EXIT_REMOVE_CHECK);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	const char* ret_val = policy.FiringExpression();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, ATTR_ON_EXIT_REMOVE_CHECK) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_exit_false() {
	emit_test("Test that FiringExpression() returns OnExitRemove after a call "
		"to AnalyzePolicy() with the PERIODIC_THEN_EXIT mode and a ClassAd that"
		" has all the checked attributes evaluate to false.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(ON_EXIT_REMOVE);
	insert_into_ad(ad, ATTR_ON_EXIT_REMOVE_CHECK, "false");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%s", ATTR_ON_EXIT_REMOVE_CHECK);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	const char* ret_val = policy.FiringExpression();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, ATTR_ON_EXIT_REMOVE_CHECK) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_value_no_init() {
	emit_test("Test that FiringExpressionValue() returns -1 when called before "
		"Init().");
	emit_input_header();
	emit_param("ClassAd", "NULL");
	emit_output_expected_header();
	emit_retval("%d", -1);
	UserPolicy policy;
	int ret_val = policy.FiringExpressionValue();
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	if(ret_val != -1) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_value_no_analyze() {
	emit_test("Test that FiringExpressionValue() returns -1 when called before "
		"AnalyzePolicy().");
	ad = new compat_classad::ClassAd();
	UserPolicy policy;
	policy.Init(ad);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%d", -1);
	int ret_val = policy.FiringExpressionValue();
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != -1) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_value_empty() {
	emit_test("Test that FiringExpressionValue() returns -1 after a call to "
		"AnalyzePolicy() with an empty ClassAd.");
	emit_input_header();
	emit_param("ClassAd", "");
	emit_output_expected_header();
	emit_retval("%d", -1);
	ad = new compat_classad::ClassAd();
	UserPolicy policy;
	policy.Init(ad);	//use default attributes
	ad->Delete(ATTR_PERIODIC_HOLD_CHECK);
	ad->Delete(ATTR_PERIODIC_RELEASE_CHECK);
	ad->Delete(ATTR_PERIODIC_REMOVE_CHECK);
	ad->Delete(ATTR_ON_EXIT_HOLD_CHECK);
	ad->Delete(ATTR_ON_EXIT_REMOVE_CHECK);
	ad->Delete(ATTR_CURRENT_TIME);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	int ret_val = policy.FiringExpressionValue();
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != -1) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_value_und_timer_remove() {
	emit_test("Test that FiringExpressionValue() returns -1 after a call to"
		" AnalyzePolicy() with a ClassAd that has TimerRemove evaluate to "
		"undefined.");
	emit_comment("See ticket #1573.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(TIMER_REMOVE);
	insert_into_ad(ad, ATTR_TIMER_REMOVE_CHECK, "DoesNotExist");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%d", -1);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	int ret_val = policy.FiringExpressionValue();
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != -1) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_value_und_periodic_hold() {
	emit_test("Test that FiringExpressionValue() returns -1 after a call to "
		"AnalyzePolicy() with a ClassAd that has PeriodicHold evaluate to "
		"undefined.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_HOLD);
	insert_into_ad(ad, ATTR_PERIODIC_HOLD_CHECK, "DoesNotExist");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%d", -1);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	int ret_val = policy.FiringExpressionValue();
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != -1) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_value_und_periodic_release() {
	emit_test("Test that FiringExpressionValue() returns -1 after a call to "
		"AnalyzePolicy() with a ClassAd that has PeriodicRelase evaluate to "
		"undefined.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_RELEASE);
	insert_into_ad(ad, ATTR_PERIODIC_RELEASE_CHECK, "DoesNotExist");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%d", -1);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	int ret_val = policy.FiringExpressionValue();
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != -1) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_value_und_periodic_remove() {
	emit_test("Test that FiringExpressionValue() returns -1 after a call to "
		"AnalyzePolicy() with a ClassAd that has PeriodicRemove evaluate to "
		"undefined.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_REMOVE);
	insert_into_ad(ad, ATTR_PERIODIC_REMOVE_CHECK, "DoesNotExist");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%d", -1);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	int ret_val = policy.FiringExpressionValue();
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != -1) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_value_und_on_exit_hold() {
	emit_test("Test that FiringExpressionValue() returns -1 after a call to "
		"AnalyzePolicy() with a ClassAd that has OnExitHold evaluate to "
		"undefined.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(ON_EXIT_HOLD);
	insert_into_ad(ad, ATTR_ON_EXIT_HOLD_CHECK, "DoesNotExist");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%d", -1);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	int ret_val = policy.FiringExpressionValue();
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != -1) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_value_und_on_exit_remove() {
	emit_test("Test that FiringExpressionValue() returns -1 after a call to "
		"AnalyzePolicy() with a ClassAd that has OnExitRemove evaluate to "
		"undefined.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(ON_EXIT_REMOVE);
	insert_into_ad(ad, ATTR_ON_EXIT_REMOVE_CHECK, "DoesNotExist");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%d", -1);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	int ret_val = policy.FiringExpressionValue();
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != -1) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_value_only_timer_remove() {
	emit_test("Test that FiringExpressionValue() returns 1 after a call to"
		" AnalyzePolicy() with the PERIODIC_ONLY mode and a ClassAd that "
		"has TimerRemove evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(TIMER_REMOVE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%d", 1);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	int ret_val = policy.FiringExpressionValue();
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != 1) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_value_only_periodic_hold() {
	emit_test("Test that FiringExpressionValue() returns 1 after a call to"
		" AnalyzePolicy() with the PERIODIC_ONLY mode and a ClassAd that "
		"has PeriodicHold evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_HOLD);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%d", 1);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	int ret_val = policy.FiringExpressionValue();
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != 1) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_value_only_periodic_release() {
	emit_test("Test that FiringExpressionValue() returns 1 after a call to"
		" AnalyzePolicy() with the PERIODIC_ONLY mode and a ClassAd that "
		"has PeriodicRelease evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_RELEASE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%d", 1);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	int ret_val = policy.FiringExpressionValue();
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != 1) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_value_only_periodic_remove() {
	emit_test("Test that FiringExpressionValue() returns 1 after a call to"
		" AnalyzePolicy() with the PERIODIC_ONLY mode and a ClassAd that "
		"has PeriodicRemove evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_REMOVE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%d", 1);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	int ret_val = policy.FiringExpressionValue();
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != 1) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_value_only_false() {
	emit_test("Test that FiringExpressionValue() returns -1 after a call to"
		"AnalyzePolicy() with the PERIODIC_ONLY mode and a ClassAd that has all"
		" the checked attributes evaluate to false.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(ON_EXIT_REMOVE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%d", -1);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	int ret_val = policy.FiringExpressionValue();
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != -1) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_value_exit_timer_remove() {
	emit_test("Test that FiringExpressionValue() returns 1 after a call to"
		" AnalyzePolicy() with the PERIODIC_THEN_EXIT mode and a ClassAd that "
		"has TimerRemove evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(TIMER_REMOVE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%d", 1);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	int ret_val = policy.FiringExpressionValue();
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != 1) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_value_exit_periodic_hold() {
	emit_test("Test that FiringExpressionValue() returns 1 after a call to"
		" AnalyzePolicy() with the PERIODIC_THEN_EXIT mode and a ClassAd that "
		"has PeriodicHold evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_HOLD);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%d", 1);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	int ret_val = policy.FiringExpressionValue();
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != 1) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_value_exit_periodic_release() {
	emit_test("Test that FiringExpressionValue() returns 1 after a call to"
		" AnalyzePolicy() with the PERIODIC_THEN_EXIT mode and a ClassAd that "
		"has PeriodicRelease evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_RELEASE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%d", 1);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	int ret_val = policy.FiringExpressionValue();
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != 1) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_value_exit_periodic_remove() {
	emit_test("Test that FiringExpressionValue() returns 1 after a call to"
		" AnalyzePolicy() with the PERIODIC_THEN_EXIT mode and a ClassAd that "
		"has PeriodicRemove evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_REMOVE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%d", 1);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	int ret_val = policy.FiringExpressionValue();
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != 1) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_value_exit_on_exit_hold() {
	emit_test("Test that FiringExpressionValue() returns 1 after a call to"
		" AnalyzePolicy() with the PERIODIC_THEN_EXIT mode and a ClassAd that "
		"has OnExitHold evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(ON_EXIT_HOLD);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%d", 1);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	int ret_val = policy.FiringExpressionValue();
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != 1) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_value_exit_on_exit_remove() {
	emit_test("Test that FiringExpressionValue() returns 1 after a call to "
		"AnalyzePolicy() with the PERIODIC_THEN_EXIT mode and a ClassAd that "
		"has OnExitRemove evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(ON_EXIT_REMOVE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%d", 1);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	int ret_val = policy.FiringExpressionValue();
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != 1) {
		FAIL;
	}
	PASS;
}

static bool test_firing_expression_value_exit_false() {
	emit_test("Test that FiringExpressionValue() returns 0 after a call to "
		"AnalyzePolicy() with the PERIODIC_THEN_EXIT mode and a ClassAd that "
		"has all the checked attributes evaluate to false.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(ON_EXIT_REMOVE);
	unparser.Unparse(classad_string, ad);
	insert_into_ad(ad, ATTR_ON_EXIT_REMOVE_CHECK, "false");
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%d", 0);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	int ret_val = policy.FiringExpressionValue();
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != 0) {
		FAIL;
	}
	PASS;
}

static bool test_firing_reason_no_init() {
	emit_test("Test that FiringReason() returns NULL when called before "
		"Init().");
	emit_input_header();
	emit_param("ClassAd", "NULL");
	emit_output_expected_header();
	emit_param("Returns NULL", "TRUE");
	UserPolicy policy;
	const char* ret_val = policy.FiringReason();
	emit_output_actual_header();
	emit_param("Returns NULL", "%s", tfstr(ret_val == NULL));
	if(ret_val != NULL) {
		FAIL;
	}
	PASS;
}

static bool test_firing_reason_no_analyze() {
	emit_test("Test that FiringReason() returns NULL when called before "
		"AnalyzePolicy().");
	ad = new compat_classad::ClassAd();
	UserPolicy policy;
	policy.Init(ad);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_param("Returns NULL", "TRUE");
	const char* ret_val = policy.FiringReason();
	emit_output_actual_header();
	emit_param("Returns NULL", "%s", tfstr(ret_val == NULL));
	CLEANUP;
	if(ret_val != NULL) {
		FAIL;
	}
	PASS;
}

static bool test_firing_reason_empty() {
	emit_test("Test that FiringReason() returns NULL after a call to "
		"AnalyzePolicy() with an empty ClassAd.");
	emit_input_header();
	emit_param("ClassAd", "");
	emit_output_expected_header();
	emit_param("Returns NULL", "TRUE");
	ad = new compat_classad::ClassAd();
	UserPolicy policy;
	policy.Init(ad);	//use default attributes
	ad->Delete(ATTR_PERIODIC_HOLD_CHECK);
	ad->Delete(ATTR_PERIODIC_RELEASE_CHECK);
	ad->Delete(ATTR_PERIODIC_REMOVE_CHECK);
	ad->Delete(ATTR_ON_EXIT_HOLD_CHECK);
	ad->Delete(ATTR_ON_EXIT_REMOVE_CHECK);
	ad->Delete(ATTR_CURRENT_TIME);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	const char* ret_val = policy.FiringReason();
	emit_output_actual_header();
	emit_param("Returns NULL", "%s", tfstr(ret_val == NULL));
	CLEANUP;
	if(ret_val != NULL) {
		FAIL;
	}
	PASS;
}

static bool test_firing_reason_undefined_timer_remove() {
	emit_test("Test that FiringReason() returns the correct reason after a call"
		" to AnalyzePolicy() with a ClassAd that has TimerRemove evaluate to "
		"undefined.");
	emit_comment("See ticket #1574.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(TIMER_REMOVE);
	insert_into_ad(ad, ATTR_TIMER_REMOVE_CHECK, "DoesNotExist");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	reason.sprintf("The job attribute %s expression 'DoesNotExist' evaluated to"
		" UNDEFINED", ATTR_TIMER_REMOVE_CHECK);
	emit_output_expected_header();
	emit_retval("%s", reason.Value());
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	const char* ret_val = policy.FiringReason();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, reason.Value()) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_reason_undefined_periodic_hold() {
	emit_test("Test that FiringReason() returns the correct reason after a call"
		" to AnalyzePolicy() with a ClassAd that has PeriodicHold evaluate to "
		"undefined.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_HOLD);
	insert_into_ad(ad, ATTR_PERIODIC_HOLD_CHECK, "DoesNotExist");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	reason.sprintf("The job attribute %s expression 'DoesNotExist' evaluated to"
		" UNDEFINED", ATTR_PERIODIC_HOLD_CHECK);
	emit_output_expected_header();
	emit_retval("%s", reason.Value());
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	const char* ret_val = policy.FiringReason();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, reason.Value()) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_reason_undefined_periodic_release() {
	emit_test("Test that FiringReason() returns the correct reason after a call"
		" to AnalyzePolicy() with a ClassAd that has PeriodicRelease evaluate "
		"to undefined.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_RELEASE);
	insert_into_ad(ad, ATTR_PERIODIC_RELEASE_CHECK, "DoesNotExist");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	reason.sprintf("The job attribute %s expression 'DoesNotExist' evaluated to"
		" UNDEFINED", ATTR_PERIODIC_RELEASE_CHECK);
	emit_output_expected_header();
	emit_retval("%s", reason.Value());
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	const char* ret_val = policy.FiringReason();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, reason.Value()) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_reason_undefined_periodic_remove() {
	emit_test("Test that FiringReason() returns the correct reason after a call"
		" to AnalyzePolicy() with a ClassAd that has PeriodicRemove evaluate "
		"to undefined.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_REMOVE);
	insert_into_ad(ad, ATTR_PERIODIC_REMOVE_CHECK, "DoesNotExist");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	reason.sprintf("The job attribute %s expression 'DoesNotExist' evaluated to"
		" UNDEFINED", ATTR_PERIODIC_REMOVE_CHECK);
	emit_output_expected_header();
	emit_retval("%s", reason.Value());
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	const char* ret_val = policy.FiringReason();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, reason.Value()) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_reason_undefined_on_exit_hold() {
	emit_test("Test that FiringReason() returns the correct reason after a call"
		" to AnalyzePolicy() with a ClassAd that has OnExitHold evaluate "
		"to undefined.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(ON_EXIT_HOLD);
	insert_into_ad(ad, ATTR_ON_EXIT_HOLD_CHECK, "DoesNotExist");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	reason.sprintf("The job attribute %s expression 'DoesNotExist' evaluated to"
		" UNDEFINED", ATTR_ON_EXIT_HOLD_CHECK);
	emit_output_expected_header();
	emit_retval("%s", reason.Value());
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	const char* ret_val = policy.FiringReason();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, reason.Value()) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_reason_undefined_on_exit_remove() {
	emit_test("Test that FiringReason() returns the correct reason after a call"
		" to AnalyzePolicy() with a ClassAd that has OnExitRemove evaluate "
		"to undefined.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(ON_EXIT_REMOVE);
	insert_into_ad(ad, ATTR_ON_EXIT_REMOVE_CHECK, "DoesNotExist");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	reason.sprintf("The job attribute %s expression 'DoesNotExist' evaluated to"
		" UNDEFINED", ATTR_ON_EXIT_REMOVE_CHECK);
	emit_output_expected_header();
	emit_retval("%s", reason.Value());
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	const char* ret_val = policy.FiringReason();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, reason.Value()) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_reason_only_timer_remove() {
	emit_test("Test that FiringReason() returns the correct reason after a call"
		" to AnalyzePolicy() with the PERIODIC_ONLY mode and a ClassAd that "
		"has TimerRemove evaluate to true.");
	emit_comment("See ticket #1575.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(TIMER_REMOVE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	reason.sprintf("The job attribute %s expression '0' evaluated to "
		"TRUE", ATTR_TIMER_REMOVE_CHECK);
	emit_output_expected_header();
	emit_retval("%s", reason.Value());
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	const char* ret_val = policy.FiringReason();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, reason.Value()) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_reason_only_periodic_hold() {
	emit_test("Test that FiringReason() returns the correct reason after a call"
		" to AnalyzePolicy() with the PERIODIC_ONLY mode and a ClassAd that "
		"has PeriodicHold evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_HOLD);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	reason.sprintf("The job attribute %s expression 'true' evaluated to "
		"TRUE", ATTR_PERIODIC_HOLD_CHECK);
	emit_output_expected_header();
	emit_retval("%s", reason.Value());
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	const char* ret_val = policy.FiringReason();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, reason.Value()) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_reason_only_periodic_release() {
	emit_test("Test that FiringReason() returns the correct reason after a call"
		" to AnalyzePolicy() with the PERIODIC_ONLY mode and a ClassAd that "
		"has PeriodicRelease evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_RELEASE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	reason.sprintf("The job attribute %s expression 'true' evaluated to "
		"TRUE", ATTR_PERIODIC_RELEASE_CHECK);
	emit_output_expected_header();
	emit_retval("%s", reason.Value());
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	const char* ret_val = policy.FiringReason();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, reason.Value()) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_reason_only_periodic_remove() {
	emit_test("Test that FiringReason() returns the correct reason after a call"
		" to AnalyzePolicy() with the PERIODIC_ONLY mode and a ClassAd that "
		"has PeriodicRemove evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_REMOVE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	reason.sprintf("The job attribute %s expression 'true' evaluated to "
		"TRUE", ATTR_PERIODIC_REMOVE_CHECK);
	emit_output_expected_header();
	emit_retval("%s", reason.Value());
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	const char* ret_val = policy.FiringReason();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, reason.Value()) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_reason_only_false() {
	emit_test("Test that FiringReason() returns NULL after a call to "
		"AnalyzePolicy() with the PERIODIC_ONLY mode and a ClassAd that has all"
		" the checked attributes evaluate to false.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(ON_EXIT_REMOVE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_param("Returns NULL", "TRUE");
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	const char* ret_val = policy.FiringReason();
	emit_output_actual_header();
	emit_param("Returns NULL", "%s", tfstr(ret_val == NULL));
	CLEANUP;
	if(ret_val != NULL) {
		FAIL;
	}
	PASS;
}

static bool test_firing_reason_exit_timer_remove() {
	emit_test("Test that FiringReason() returns the correct reason after a call"
		" to AnalyzePolicy() with the PERIODIC_THEN_EXIT mode and a ClassAd "
		"that has TimerRemove evaluate to true.");
	emit_comment("See ticket #1575.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(TIMER_REMOVE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	reason.sprintf("The job attribute %s expression '0' evaluated to TRUE", 
		ATTR_TIMER_REMOVE_CHECK);
	emit_output_expected_header();
	emit_retval("%s", reason.Value());
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	const char* ret_val = policy.FiringReason();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, reason.Value()) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_reason_exit_periodic_hold() {
	emit_test("Test that FiringReason() returns the correct reason after a call"
		" to AnalyzePolicy() with the PERIODIC_THEN_EXIT mode and a ClassAd "
		"that has PeriodicHold evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_HOLD);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	reason.sprintf("The job attribute %s expression 'true' evaluated to TRUE", 
		ATTR_PERIODIC_HOLD_CHECK);
	emit_output_expected_header();
	emit_retval("%s", reason.Value());
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	const char* ret_val = policy.FiringReason();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, reason.Value()) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_reason_exit_periodic_release() {
	emit_test("Test that FiringReason() returns the correct reason after a call"
		" to AnalyzePolicy() with the PERIODIC_THEN_EXIT mode and a ClassAd "
		"that has PeriodicRelease evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_RELEASE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	reason.sprintf("The job attribute %s expression 'true' evaluated to TRUE", 
		ATTR_PERIODIC_RELEASE_CHECK);
	emit_output_expected_header();
	emit_retval("%s", reason.Value());
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	const char* ret_val = policy.FiringReason();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, reason.Value()) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_reason_exit_periodic_remove() {
	emit_test("Test that FiringReason() returns the correct reason after a call"
		" to AnalyzePolicy() with the PERIODIC_THEN_EXIT mode and a ClassAd "
		"that has PeriodicRemove evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_REMOVE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	reason.sprintf("The job attribute %s expression 'true' evaluated to TRUE", 
		ATTR_PERIODIC_REMOVE_CHECK);
	emit_output_expected_header();
	emit_retval("%s", reason.Value());
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	const char* ret_val = policy.FiringReason();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, reason.Value()) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_reason_exit_on_exit_hold() {
	emit_test("Test that FiringReason() returns the correct reason after a call"
		" to AnalyzePolicy() with the PERIODIC_THEN_EXIT mode and a ClassAd "
		"that has OnExitHold evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(ON_EXIT_HOLD);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	reason.sprintf("The job attribute %s expression 'true' evaluated to TRUE", 
		ATTR_ON_EXIT_HOLD_CHECK);
	emit_output_expected_header();
	emit_retval("%s", reason.Value());
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	const char* ret_val = policy.FiringReason();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, reason.Value()) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_reason_exit_on_exit_remove() {
	emit_test("Test that FiringReason() returns the correct reason after a call"
		" to AnalyzePolicy() with the PERIODIC_THEN_EXIT mode and a ClassAd "
		"that has OnExitRemove evaluate to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(ON_EXIT_REMOVE);
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	reason.sprintf("The job attribute %s expression 'true' evaluated to TRUE",
		ATTR_ON_EXIT_REMOVE_CHECK);
	emit_output_expected_header();
	emit_retval("%s", reason.Value());
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	const char* ret_val = policy.FiringReason();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, reason.Value()) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_firing_reason_exit_false() {
	emit_test("Test that FiringReason() returns the correct reason after a call"
		" to AnalyzePolicy() with the PERIODIC_THEN_EXIT mode and a ClassAd "
		"that has all the checked attributes evaluate to false.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(ON_EXIT_REMOVE);
	insert_into_ad(ad, ATTR_ON_EXIT_REMOVE_CHECK, "false");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	reason.sprintf("The job attribute %s expression 'false' evaluated to FALSE",
		ATTR_ON_EXIT_REMOVE_CHECK);
	emit_output_expected_header();
	emit_retval("%s", reason.Value());
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_THEN_EXIT);
	const char* ret_val = policy.FiringReason();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, reason.Value()) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_remove_macro_analyze_policy() {
	emit_test("Test that AnalyzePolicy() returns REMOVE_FROM_QUEUE when used"
		" with the PERIODIC_ONLY mode and a ClassAd that has PeriodicRemove "
		"evaluate to false, but SYSTEM_PERIODIC_REMOVE evaluates to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_REMOVE);
	insert_into_ad(ad, ATTR_PERIODIC_REMOVE_CHECK, "false");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%d", REMOVE_FROM_QUEUE);
	param_info_insert("SYSTEM_PERIODIC_REMOVE", NULL, "true", NULL, ".*", 
					  0, 0, 0, 0, 0, NULL, NULL, NULL, NULL);
	UserPolicy policy;
	policy.Init(ad);
	int ret_val = policy.AnalyzePolicy(PERIODIC_ONLY);
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != REMOVE_FROM_QUEUE) {
		FAIL;
	}
	PASS;
}

static bool test_remove_macro_firing_expression() {
	emit_test("Test that FiringExpression() returns SYSTEM_PERIODIC_REMOVE "
		"after a call to AnalyzePolicy() with the PERIODIC_ONLY mode and a "
		"ClassAd that has PeriodicRemove evaluate to false, but "
		"SYSTEM_PERIODIC_REMOVE evaluates to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_REMOVE);
	insert_into_ad(ad, ATTR_PERIODIC_REMOVE_CHECK, "false");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("SYSTEM_PERIODIC_REMOVE");
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	const char* ret_val = policy.FiringExpression();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, "SYSTEM_PERIODIC_REMOVE") != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_remove_macro_firing_expression_value() {
	emit_test("Test that FiringExpressionValue() returns 1 after a call to"
		" AnalyzePolicy() with the PERIODIC_ONLY mode and a ClassAd that "
		"has PeriodicRemove evaluate to false, but SYSTEM_PERIODIC_REMOVE "
		"evaluates to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_REMOVE);
	insert_into_ad(ad, ATTR_PERIODIC_REMOVE_CHECK, "false");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%d", 1);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	int ret_val = policy.FiringExpressionValue();
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != 1) {
		FAIL;
	}
	PASS;
}

static bool test_remove_macro_firing_reason() {
	emit_test("Test that FiringReason() returns the correct reason after a call"
		" to AnalyzePolicy() with the PERIODIC_ONLY mode and a ClassAd "
		"that has PeriodicRemove evaluate to false, but SYSTEM_PERIODIC_REMOVE "
		"evaluates to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_REMOVE);
	insert_into_ad(ad, ATTR_PERIODIC_REMOVE_CHECK, "false");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	reason.sprintf("The system macro SYSTEM_PERIODIC_REMOVE expression 'true' "
		"evaluated to TRUE");
	emit_output_expected_header();
	emit_retval("%s", reason.Value());
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	const char* ret_val = policy.FiringReason();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, reason.Value()) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_release_macro_analyze_policy() {
	emit_test("Test that AnalyzePolicy() returns RELEASE_FROM_QUEUE when used"
		" with the PERIODIC_ONLY mode and a ClassAd that has PeriodicRelease "
		"evaluate to false, but SYSTEM_PERIODIC_RELEASE evaluates to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_RELEASE);
	insert_into_ad(ad, ATTR_PERIODIC_RELEASE_CHECK, "false");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%d", RELEASE_FROM_HOLD);
	param_info_insert("SYSTEM_PERIODIC_RELEASE", NULL, "true", NULL, ".*", 
					  0, 0, 0, 0, 0, NULL, NULL, NULL, NULL);
	UserPolicy policy;
	policy.Init(ad);
	int ret_val = policy.AnalyzePolicy(PERIODIC_ONLY);
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != RELEASE_FROM_HOLD) {
		FAIL;
	}
	PASS;
}

static bool test_release_macro_firing_expression() {
	emit_test("Test that FiringExpression() returns SYSTEM_PERIODIC_RELEASE "
		"after a call to AnalyzePolicy() with the PERIODIC_ONLY mode and a "
		"ClassAd that has PeriodicRelease evaluate to false, but "
		"SYSTEM_PERIODIC_RELEASE evaluates to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_RELEASE);
	insert_into_ad(ad, ATTR_PERIODIC_RELEASE_CHECK, "false");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("SYSTEM_PERIODIC_RELEASE");
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	const char* ret_val = policy.FiringExpression();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, "SYSTEM_PERIODIC_RELEASE") != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_release_macro_firing_expression_value() {
	emit_test("Test that FiringExpressionValue() returns 1 after a call to"
		" AnalyzePolicy() with the PERIODIC_ONLY mode and a ClassAd that "
		"has PeriodicRelease evaluate to false, but SYSTEM_PERIODIC_RELEASE "
		"evaluates to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_RELEASE);
	insert_into_ad(ad, ATTR_PERIODIC_RELEASE_CHECK, "false");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%d", 1);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	int ret_val = policy.FiringExpressionValue();
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != 1) {
		FAIL;
	}
	PASS;
}

static bool test_release_macro_firing_reason() {
	emit_test("Test that FiringReason() returns the correct reason after a call"
		" to AnalyzePolicy() with the PERIODIC_ONLY mode and a ClassAd "
		"that has PeriodicRelease evaluate to false, but SYSTEM_PERIODIC_RELEASE "
		"evaluates to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_RELEASE);
	insert_into_ad(ad, ATTR_PERIODIC_RELEASE_CHECK, "false");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	reason.sprintf("The system macro SYSTEM_PERIODIC_RELEASE expression 'true' "
		"evaluated to TRUE");
	emit_output_expected_header();
	emit_retval("%s", reason.Value());
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	const char* ret_val = policy.FiringReason();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, reason.Value()) != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_hold_macro_analyze_policy() {
	emit_test("Test that AnalyzePolicy() returns RELEASE_FROM_QUEUE when used"
		" with the PERIODIC_ONLY mode and a ClassAd that has PeriodicHold "
		"evaluate to false, but SYSTEM_PERIODIC_HOLD evaluates to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_HOLD);
	insert_into_ad(ad, ATTR_PERIODIC_HOLD_CHECK, "false");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%d", HOLD_IN_QUEUE);
	param_info_insert("SYSTEM_PERIODIC_HOLD", NULL, "true", NULL, ".*", 
					  0, 0, 0, 0, 0, NULL, NULL, NULL, NULL);
	UserPolicy policy;
	policy.Init(ad);
	int ret_val = policy.AnalyzePolicy(PERIODIC_ONLY);
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != HOLD_IN_QUEUE) {
		FAIL;
	}
	PASS;
}

static bool test_hold_macro_firing_expression() {
	emit_test("Test that FiringExpression() returns SYSTEM_PERIODIC_HOLD "
		"after a call to AnalyzePolicy() with the PERIODIC_ONLY mode and a "
		"ClassAd that has PeriodicHold evaluate to false, but "
		"SYSTEM_PERIODIC_HOLD evaluates to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_HOLD);
	insert_into_ad(ad, ATTR_PERIODIC_HOLD_CHECK, "false");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("SYSTEM_PERIODIC_HOLD");
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	const char* ret_val = policy.FiringExpression();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, "SYSTEM_PERIODIC_HOLD") != MATCH) {
		FAIL;
	}
	PASS;
}

static bool test_hold_macro_firing_expression_value() {
	emit_test("Test that FiringExpressionValue() returns 1 after a call to"
		" AnalyzePolicy() with the PERIODIC_ONLY mode and a ClassAd that "
		"has PeriodicHold evaluate to false, but SYSTEM_PERIODIC_HOLD "
		"evaluates to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_HOLD);
	insert_into_ad(ad, ATTR_PERIODIC_HOLD_CHECK, "false");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	emit_output_expected_header();
	emit_retval("%d", 1);
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	int ret_val = policy.FiringExpressionValue();
	emit_output_actual_header();
	emit_retval("%d", ret_val);
	CLEANUP;
	if(ret_val != 1) {
		FAIL;
	}
	PASS;
}

static bool test_hold_macro_firing_reason() {
	emit_test("Test that FiringReason() returns the correct reason after a call"
		" to AnalyzePolicy() with the PERIODIC_ONLY mode and a ClassAd "
		"that has PeriodicHold evaluate to false, but SYSTEM_PERIODIC_HOLD "
		"evaluates to true.");
	ad = new compat_classad::ClassAd();
	ad->initFromString(PERIODIC_HOLD);
	insert_into_ad(ad, ATTR_PERIODIC_HOLD_CHECK, "false");
	unparser.Unparse(classad_string, ad);
	emit_input_header();
	emit_param("ClassAd", "%s", classad_string.c_str());
	reason.sprintf("The system macro SYSTEM_PERIODIC_HOLD expression 'true' "
		"evaluated to TRUE");
	emit_output_expected_header();
	emit_retval("%s", reason.Value());
	UserPolicy policy;
	policy.Init(ad);
	policy.AnalyzePolicy(PERIODIC_ONLY);
	const char* ret_val = policy.FiringReason();
	emit_output_actual_header();
	emit_retval("%s", ret_val);
	CLEANUP;
	if(strcmp(ret_val, reason.Value()) != MATCH) {
		FAIL;
	}
	PASS;
}