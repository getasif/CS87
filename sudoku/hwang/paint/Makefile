LINCOMPILE = /usr/bin/g++ -c -v -g -Wall
LINLINK = /usr/bin/g++ -v

SUNCOMPILE = /usr/sfw/bin/g++ -c -v -g -Wall
SUNLINK = /usr/sfw/bin/g++ -v

binLinux/paint: ofilesLinux/paint.o
	$(LINLINK) -o binLinux/paint ofilesLinux/paint.o

binLinux/paint_con: ofilesLinux/paint_con.o
	$(LINLINK) -o binLinux/paint_con ofilesLinux/paint_con.o

binLinux/pro_to_txt: ofilesLinux/pro_to_txt.o
	$(LINLINK) -o binLinux/pro_to_txt ofilesLinux/pro_to_txt.o

binLinux/txt_to_pro: ofilesLinux/txt_to_pro.o
	$(LINLINK) -o binLinux/txt_to_pro ofilesLinux/txt_to_pro.o

ofilesLinux/paint.o: source/paint.c
	$(LINCOMPILE) -o ofilesLinux/paint.o source/paint.c

ofilesLinux/paint_con.o: source/paint_con.c
	$(LINCOMPILE) -o ofilesLinux/paint_con.o source/paint_con.c

ofilesLinux/pro_to_txt.o: source/pro_to_txt.c
	$(LINCOMPILE) -o ofilesLinux/pro_to_txt.o source/pro_to_txt.c

ofilesLinux/txt_to_pro.o: source/txt_to_pro.c
	$(LINCOMPILE) -o ofilesLinux/txt_to_pro.o source/txt_to_pro.c


binSun/paint: ofilesSun/paint.o
	$(SUNLINK) -o binSun/paint ofilesSun/paint.o

binSun/paint_con: ofilesSun/paint_con.o
	$(SUNLINK) -o binSun/paint_con ofilesSun/paint_con.o

binSun/pro_to_txt: ofilesSun/pro_to_txt.o
	$(SUNLINK) -o binSun/pro_to_txt ofilesSun/pro_to_txt.o

binSun/txt_to_pro: ofilesSun/txt_to_pro.o
	$(SUNLINK) -o binSun/txt_to_pro ofilesSun/txt_to_pro.o

ofilesSun/paint.o: source/paint.c
	$(SUNCOMPILE) -o ofilesSun/paint.o source/paint.c

ofilesSun/paint_con.o: source/paint_con.c
	$(SUNCOMPILE) -o ofilesSun/paint_con.o source/paint_con.c

ofilesSun/pro_to_txt.o: source/pro_to_txt.c
	$(SUNCOMPILE) -o ofilesSun/pro_to_txt.o source/pro_to_txt.c

ofilesSun/txt_to_pro.o: source/txt_to_pro.c
	$(SUNCOMPILE) -o ofilesSun/txt_to_pro.o source/txt_to_pro.c

