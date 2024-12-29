#include "sendmail.h"
#include <stdio.h>
#include <unistd.h>
#include <syslog.h>

int main(void)
{
	struct MailInfo mail = {{ '\0', '\0', '\0' }};
	bool check_mail = false;
	bool has_mail_info = false;
	bool send_mail_state = false;

	openlog("AutoReplyMail", LOG_ODELAY, LOG_USER);

	for (;;) {
		check_mail = check_new_mail(&mail);

		if (check_mail == true) {
			has_mail_info = get_mail_info(&mail);

			check_mail = false;
		}

		if (has_mail_info == true) {
			send_mail_state = send_mail(&mail);

			if (send_mail_state == false) {
				syslog(LOG_INFO, "can not send mail\n");
			}
			else if (send_mail_state == true) {
				syslog(LOG_INFO, "succsesful send mail\n");
			}

			reset_mail_info(&mail);

			send_mail_state = false;
			has_mail_info = false;
		}

		sleep(1);
	}

	return 0;
}
