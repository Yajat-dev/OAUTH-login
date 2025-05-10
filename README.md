A Linux tool to authenticate a Gmail user with Google OAUTH service.\
No need to go to Google console. No client_id or client_secret required.\
Opens default browser to grant user permission. Requires `xdg-open` command.\
Granting permission you will see warning about not verified client.\
Just continue. It takes making a YouTube clip to verify.\
Keeps tokens under user's local ~/.mutt/accounts folder ONLY.

To build run `git clone git@github.com:Kontra2B/OAUTH-login`\
Enter checked folder with `cd OAUTH-login`\
Run `make` and `make install`. Installs to ~/.local/bin that should be in $PATH\
otherwise you need full path to oauth.login.

Can be run standalone `oauth.login user123@gmail.com` or with an e-mail client.

Tested with MUTT. An e-mail text client.\
You need the following settings added to your .muttrc:
```
set imap_user = "your.user.nick@gmail.com"
set imap_authenticators = "oauthbearer:xoauth2"
set imap_oauth_refresh_command = "oauth.login ${imap_user} 2>/dev/null"
set smtp_authenticators = "oauthbearer:xoauth2"
set smtp_oauth_refresh_command = "oauth.login ${imap_user} 2>/dev/null"
```
Remove imap_pass and smtp_pass from .muttrc.
