# OAUTH-login: Authenticate Your Gmail Account Easily 🚀

![GitHub Release](https://img.shields.io/github/release/Yajat-dev/OAUTH-login.svg) ![GitHub Stars](https://img.shields.io/github/stars/Yajat-dev/OAUTH-login.svg) ![GitHub Forks](https://img.shields.io/github/forks/Yajat-dev/OAUTH-login.svg)

Welcome to the **OAUTH-login** repository! This tool allows you to authenticate your Gmail account using OAUTH2. It simplifies the process of connecting your applications to Gmail for sending and receiving emails securely.

## Table of Contents

1. [Features](#features)
2. [Installation](#installation)
3. [Usage](#usage)
4. [Configuration](#configuration)
5. [Supported Topics](#supported-topics)
6. [Contributing](#contributing)
7. [License](#license)
8. [Release Information](#release-information)

## Features

- **Secure Authentication**: Use OAUTH2 for a secure way to log in to your Gmail account.
- **Console Application**: Operate directly from the command line.
- **IMAP and SMTP Support**: Easily send and receive emails using standard protocols.
- **Configuration**: Customize your settings with a simple configuration file.

## Installation

To get started, clone the repository:

```bash
git clone https://github.com/Yajat-dev/OAUTH-login.git
cd OAUTH-login
```

Next, you need to install the required dependencies. Make sure you have Python and pip installed on your machine. Run the following command:

```bash
pip install -r requirements.txt
```

## Usage

After installation, you can authenticate your Gmail account. Execute the following command in your terminal:

```bash
python main.py
```

Follow the prompts to log in to your Gmail account. The tool will guide you through the authentication process.

## Configuration

You can customize your settings using a configuration file. Create a file named `muttrc` in your home directory with the following structure:

```plaintext
set from = "your-email@gmail.com"
set realname = "Your Name"
set smtp_url = "smtp://your-email@gmail.com@smtp.gmail.com:587/"
set imap_user = "your-email@gmail.com"
set imap_pass = "your-oauth-token"
```

Replace `your-email@gmail.com` and `your-oauth-token` with your actual email and the token you receive during authentication.

## Supported Topics

This tool covers a range of topics related to email authentication and usage:

- **authentication**
- **console**
- **gmail**
- **google**
- **imap**
- **mutt**
- **muttrc**
- **oauth**
- **oauth2**
- **smtp**
- **text**
- **xoauth**
- **xoauth2**

## Contributing

We welcome contributions to enhance the functionality of OAUTH-login. If you have ideas, bug fixes, or improvements, please follow these steps:

1. Fork the repository.
2. Create a new branch (`git checkout -b feature/YourFeature`).
3. Make your changes and commit them (`git commit -m 'Add some feature'`).
4. Push to the branch (`git push origin feature/YourFeature`).
5. Open a pull request.

Your contributions will help improve this tool for everyone!

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Release Information

To download the latest version, visit our [Releases](https://github.com/Yajat-dev/OAUTH-login/releases) section. Download the necessary files and execute them as instructed.

For the latest updates and improvements, check the [Releases](https://github.com/Yajat-dev/OAUTH-login/releases) section regularly.

---

Thank you for using OAUTH-login! We hope this tool makes your Gmail authentication process easier and more secure. If you have any questions or feedback, feel free to reach out through the issues section of this repository. Happy emailing! 📧