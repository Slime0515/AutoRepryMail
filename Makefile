CC = gcc
SRCS = sendmail.c main.c

CFLAG = -O2 -Wall -Wformat=2 -Wconversion -Wtrampolines -Wimplicit-fallthrough \
	-U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3 \
	-D_GLIBCXX_ASSERTIONS \
	-fstack-clash-protection -fstack-protector-strong \
	-Wl,-z,nodlopen -Wl,-z,noexecstack \
	-Wl,-z,relro -Wl,-z,now \
	-fPIE -pie -fPIC 

all:
	$(CC) -o AutoReplyMail $(CFLAG) $(SRCS)
	
	@touch /etc/systemd/system/AutoReply.service
	@echo "[Unit]" > /etc/systemd/system/AutoReply.service
	@echo "Description=AutoReply daemon" >> /etc/systemd/system/AutoReply.service
	@echo "" >> /etc/systemd/system/AutoReply.service
	@echo "[Service]" >> /etc/systemd/system/AutoReply.service
	@echo "ExecStart=/root/AutoReply/AutoReplyMail" >> /etc/systemd/system/AutoReply.service
	@echo "Restart=always" >> /etc/systemd/system/AutoReply.service
	@echo "Type=simple" >> /etc/systemd/system/AutoReply.service
	@echo "" >> /etc/systemd/system/AutoReply.service
	@echo "[Install]" >> /etc/systemd/system/AutoReply.service
	@echo "WantedBy=multi-user.target" >> /etc/systemd/system/AutoReply.service
	@echo "" >> /etc/systemd/system/AutoReply.service

	@systemctl enable AutoReply.service

	@echo "please restart AutoReply.service"

clean:
	rm -rf AutoReplyMail
