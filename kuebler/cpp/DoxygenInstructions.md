# Installation


1. Open a Terminal and install make
	
	```console
	user@computer: sudo apt-get install make
	```

2. Install doxygen
	
	```console
	user@computer: sudo apt-get install doxygen
	```

3. Install graphviz

	```console
	user@computer: sudo apt-get install graphviz
	```

4. Install LaTex

	```console
	user@computer: sudo apt-get install texlive-latex-base texlive-latex-extra
	```

5. Install npm and moxygen

	```console
	user@computer: sudo apt-get install npm
	user@computer: sudo npm install moxygen -g
	```



# Usage

1. Go to your project home

	```console
	user@computer: cd /path/to/project/home
	```

2. Create a documentaion directory and enter it

	```console
	user@computer: mkdir <documentation-directory>
	user@computer: cd <documentation-directory>
	```

3. Generate doxygen config file

	```console
	user@computer: doxygen -g <config-name>.conf
	```

4. Edit config file

	There are a few basic settings that need to be changed in order to generate graphics.

	```console
	HAVE_DOT = YES
	CALL_GRAPH = YES
	CALLER_GRAPH = YES
	EXTRACT_ALL = YES
	```

	Set project name and a brief description for the documentation title.

	```console
	PROJECT_NAME = "your project name"
	PROJECT_BRIEF = "brief project description"
	```

	Set all the directories you want to include in the documentation.

	```console
	INPUT = ../include ../src
	```

	Set the output directory.

	```console
	OUTPUT_DIRECTORY = "./"
	```

	Choose the types of output you want to generate.

	The html output generates an index.html file that can be viewed with a browser.
	```console
	GENERATE_HTML = YES
	HTML_OUTPUT = html/
	```

	The LaTex output can be build in order to generate a PDF file. 
	The generated file can be found in the latex directory as refman.pdf.
	```console
	GENERATE_LATEX= YES
	LATEX_OUTPUT = latex/
	```

	The xml output can be used to generate markdown files for git.
	```console
	GENERATE_XML = YES
	XML_OUTPUT = xml/
	```

5. Open Terminal and go to your project documentation directory

	If you want to call doxygen from an other directory you need to change all paths to absolut paths in the doxygen config.

	```console
	user@computer: cd /path/to/project/home/<documentation-directory>
	```

6. Execute doxygen

	```console
	user@computer: doxygen <config-name>.conf
	```

7. Generate PDF file from LaTex output

	```console
	user@computer: cd <latex-directory>
	user@computer: make
	user@computer: cp refman.pdf ../
	```

8. Generate markdown file from xml output

	```console
	user@computer: cd ..
	user@computer: moxygen -o <markdown-name>.md ./<xml-directory>
	```


# Output 

If ALL previously described instructions were followed
the resulting structure should resemble the following file tree.

```
/path/to/project/home/
│
└───<documentation-directory>
│   │   <config-name>.conf
│   │   refman.pdf
│   │   <markdown-name>.md
│   │
│   └───<html-directory>
│   │   │   index.html
│   │   │   ...
│   │   
│   └───<latex-directory>
│   │   │	refman.tex
│   │   │   ...
│   │   
│   └───<xml-directory>
│       │   ...
│       │	...
│       │   ...
│   
│	...
│
└───...
└───...
```

# Import to GitLab

The markdown file can be pushed into the git wiki to be included in the git documentation.

1. Create and enter git directory

	```console
	user@computer: mkdir <git-directory>
	user@computer: cd <git-directory>
	```

2. Clone and enter wiki repository

	:exclamation: "<repository-name>.wiki" is the name of the wiki repository.

	```console
	user@computer: git clone https://gitlab.tubit.tu-berlin.de/MURMEL/<repository-name>.wiki.git
	user@computer: cd <repository-name>.wiki
	```

3. Copy markdown file into wiki repository

	Markdown files can be generated using doxygen and moxygen.

	```console
	user@computer: cp /path/to/project/home/<documentation-directory>/<markdown-name>.md ./
	```

4. Add new files to git

	```console
	user@computer: git add ./*
	```

5. Commit local changes

	```console
	user@computer: git commit -m "commit message"
	```

6. Push changes to git

	```console
	user@computer: git push origin master
	```

6. Link to a markdown file

	GitLab allows to link markdown files using page-slugs.

	```console
	[Link Title](page-slug)
	```

	page-slugs can be derived from the any file name within the git.
	- the file ending has to be removed
	- only the first letter can be capital sized


	```
	<git-directory>/<repository-name>.wiki
	│
	│    home.md
	│    Test.md
	│    TestCamelCase.md
	```
	
	```console
	[Link Title](home)
	[Link Title](Test)
	[Link Title](Testcamelcase)
	```
	