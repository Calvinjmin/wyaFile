# wyaFile

| **A powerful indexing tool for your local files!**

**wyaFile** empowers you to quickly explore and analyze your files. Imagine grep, but better.

## Installation

### Local

```bash
# Use the provided shell script to install package
./install.sh
```

After installation, you can use `wya` from anywhere in your terminal!

## Usage

Once installed, use the `wya` command from anywhere:

```bash
# Search for files containing a keyword
wya scan -key basic --allow

# Scan a specific directory for .txt files
wya scan -dir /path/to/directory --allow

# Show help
wya help
```

## Available Commands

- `wya scan -key <keyword> --allow` - Search for files containing a keyword
- `wya scan -dir <path> --allow` - Scan a directory for .txt files
- `wya help` - Show available commands

**Note:** The `--allow` flag is required for security when accessing directories.

## Video Demo

![wyaFile Demo](static/scan%20examples.gif)

**Note:** This is an old video.

## Roadmap & Future Vision

wyaFile's long-term vision is to become a quick indexing tool for file storages.

- **Replaces Generic System Search:** Offering a more efficient, customizable, and intelligent alternative.
- **Handles Diverse File Types:** Expanding beyond `.txt` to support PDFs, Word documents, and other common formats.
- **Delivers Advanced Search:** Implementing intelligent indexing and ranking algorithms for highly relevant results.
- **AI Integration:** Searching via keywords will include AI to enhance the 'fuzzy-search' functionality.

### Next Steps

I am actively working on:

- **Package Creation:** Plan to ship `wya` as a package, downloadable via `brew`.
- **AI Integration:** Using open-source models, like Llama or Gemini, to enhance the searching capabilities.
- **Enhanced Interface:** Exploring improvements to the command-line interface for better usability and feedback.

## License

This project is open source and available under the [MIT License](LICENSE).

This means you're free to use, modify, and distribute wyaFile for both commercial and non-commercial purposes, with attribution. Please see the [LICENSE](LICENSE) file for the full text.
