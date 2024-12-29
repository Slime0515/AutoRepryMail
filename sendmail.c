#include "sendmail.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <syslog.h>

#include "sendmail.config"

//#define DEBUG_MODE

#ifdef DEBUG_MODE
	#define DEBUG() printf("%s\n", __func__)
#else
	#define DEBUG() ;
#endif

bool send_mail(struct MailInfo* info)
{
	const char *FROM = MY_ADDRESS;
	FILE *sendmail_p = NULL;
	char command[256] = { '\0' };

	DEBUG();	

	if (*(info->mail_from) == '\0') {
		syslog(LOG_INFO, "mail send faile");

		return false;
	}

	snprintf(command, sizeof(command), "%s -f %s -t %s", MAIL_COMMAND, FROM, (info->mail_from));

	if(strstr(command, "-f@")){
		syslog(LOG_INFO, "unkown mail address");

		return false;
	}

	sendmail_p = popen(command, "w");
	if(sendmail_p == NULL){
		return false;
	}

	fprintf(sendmail_p, "From: %s\n", FROM);
	fprintf(sendmail_p, "To: %s\n", info->mail_from);
	fprintf(sendmail_p, "Subject: %s\n", SUBJECT);
	fprintf(sendmail_p, "%s\n", BODY_STRING);
	fprintf(sendmail_p, ".\n");

	pclose(sendmail_p);

	return true;
}

bool check_new_mail(struct MailInfo* info)
{
	FILE *ls_txt_fp = NULL;
	int system_result = 1;
	int ls_result = 0;

	char command[256] = { '\0' };

	snprintf(command, sizeof(command), "ls %s -1 > /root/AutoReply/ls.txt", TARGET_MAILDIR);

	DEBUG();	

	system_result = system(command);
	if(system_result == 1) {
		syslog(LOG_INFO, "system command is false in check_new_mail function");

		return false;
	}

	ls_txt_fp = fopen("/root/AutoReply/ls.txt", "r");
	if (feof(ls_txt_fp)) {
		return false;
	}
	ls_result = fscanf(ls_txt_fp, "%s", info->mail_name);

	fclose(ls_txt_fp);

	if (strchr(info->mail_name, '.') == NULL || ls_result == 0) {
		return false;
	}
	else {
		syslog(LOG_INFO, "get new mail");

		return true;
	}
}

bool get_mail_from(struct MailInfo* info)
{
	FILE* mail_file_fp = NULL;
	char mail_path[256] = TARGET_MAILDIR; 
	int get_from = 0;
	char command[256] = { '\0' };

	DEBUG();	

	strcat(mail_path, info->mail_name);

	mail_file_fp = fopen(mail_path, "r");
	if(mail_file_fp == NULL) {
		syslog(LOG_INFO, "no such file or directory");

		return false;
	}
	get_from = fscanf(mail_file_fp, "Return-Path: <%s>", info->mail_from);
	if(get_from == 0) {
		syslog(LOG_WARNING, "can't get Return-Path");

		return false;
	}

	fclose(mail_file_fp);

	if(*(info->mail_from) == '-') {
		reset_mail_info(info);

		char rm_command[256] = { '\0' };
		snprintf(rm_command, sizeof(rm_command), "rm %s*", TARGET_MAILDIR);

		if(system(rm_command));

		syslog(LOG_WARNING, "get unknown mail");

		return false;
	}

	char *p = strchr(info->mail_from, '>');
	if(p == NULL){
		;
	}
	else{
		*p = '\0';
	}

	DEBUG();

	if (*(info->mail_from) == '\0') {
		syslog(LOG_INFO, "get from faile");

		return false;
	}
	else {
		snprintf(command, sizeof(command), "mv %s %s", mail_path, MOV_MAILDIR);

		if(system(command));

		syslog(LOG_INFO, "New mail have been moved");

		return true;
	}
}

bool get_mail_info(struct MailInfo* info)
{
	bool mail_from_state = false;

	DEBUG();	

	mail_from_state = get_mail_from(info);
	if(mail_from_state == false) {
		syslog(LOG_INFO, "mail info is nothing\n");

		return false;
	}
	else {
		return true;
	}
}

void reset_mail_info(struct MailInfo* info)
{
	DEBUG();	

	memset(info->mail_from, '\0', 128);
	memset(info->mail_name, '\0', 128);
	memset(info->mail_path, '\0', 128);
}

