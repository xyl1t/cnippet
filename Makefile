build: main.c
	gcc -o cnippet main.c

debug: main.c
	gcc -o cnippet -DDEBUG main.c

install:
	chmod +x ./cnippet
	ln -s ${PWD}/cnippet ~/.config/helix/cnippet 
	cp -n snippets.txt ~/.config/helix

uninstall:
	rm ~/.config/helix/cnippet
