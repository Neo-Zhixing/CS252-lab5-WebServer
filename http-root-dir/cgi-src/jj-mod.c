/*
 * Submarine ordering form through FAX gateway
 * 
 * Rob McCool
 * 
 */


#include <stdio.h>
#include <string.h>
#ifndef NO_STDLIB_H
#include <stdlib.h>
#else
char *getenv();
#endif

#if 1
#define JJ_FAX "JIMMY_JOHNS_5555680@fax.uiuc.edu"
#else
#define JJ_FAX "wow@foo.bar.com"
#endif

#define PASSWORD "HTTPdRocKs"
#define LF 10

void getword(char *word, const char *line, char stop);
char x2c(char *what);
void unescape_url(char *url);
void plustospace(char *str);


char *sublist[] = {
    "The Pepe Gourmet Sub",
    "Big John Gourmet Sub",
    "Sorry Charlie Gourmet Sub",
    "Turkey Tom Gourmet Sub",
    "Vito Gourmet Sub",
    "Vegetarian Gourmet Sub",
    "Gourmet Smoked Ham Club",
    "Billy Club",
    "Italian Night Club",
    "Hunter's Club",
    "Country Club",
    "The Beach Club"
    };

char *slimlist[] = {
    "Ham and Cheese",
    "Rare Roast Beef",
    "California Tuna",
    "Sliced Turkey",
    "Salami and Capacola",
    "Double Provolone"
    };

char *sidelist[] = {
    "Lay's Potato Chips",
    "Jumbo Kosher Dill"
    };

char *poplist[] = {
    "Pepsi",
    "Mountain Dew",
    "Diet Pepsi",
    "Iced Tea"
    };

void dump_form(FILE* fssock) {
    fprintf(fssock, "<TITLE>Form for Submarine Order</TITLE>%c",LF);
    fprintf(fssock, "<H1>Jimmy John's Submarine Order Form</H1>%c",LF);
    fprintf(fssock, "This form will send a faxed order to Jimmy John's in Champaign. Proper password is requred%c",LF);
    fprintf(fssock, "for order to be submitted, otherwise a copy of the order that would have been submitted will%c",LF);
    fprintf(fssock, "will be displayed.<P>%c",LF);
    fprintf(fssock, "<HR>%c",LF);
    fprintf(fssock, "<FORM ACTION=\"jj\">%c",LF);
    fprintf(fssock, "Password: <INPUT TYPE=\"text\" NAME=\"pwd\" MAXLENGTH=\"20\"><P>%c",LF);
    fprintf(fssock, "<H3>Sub Type</H3>%c",LF);
    fprintf(fssock, "Select which you would like of the following:<P>%c",LF);
    fprintf(fssock, "<INPUT TYPE=\"checkbox\" NAME=\"sub\" VALUE=\"0\">%s:%c",sublist[0],LF);
    fprintf(fssock, "Smoked virginia ham and provolone cheese topped with lettuce, tomato, and mayo.<P>%c",LF);
    fprintf(fssock, "<INPUT TYPE=\"checkbox\" NAME=\"sub\" VALUE=\"1\">%s:%c",sublist[1],LF);
    fprintf(fssock, "Medium rare shaved roast beef topped with mayo, lettuce, and tomato.<P>%c",LF);
    fprintf(fssock, "<INPUT TYPE=\"checkbox\" NAME=\"sub\" VALUE=\"2\">%s:%c",sublist[2],LF);
    fprintf(fssock, "Tuna, mixed with celery, onions, and sauce, topped with lettuce,%c",LF);
    fprintf(fssock, "tomato, and alfalfa sprouts.<P>%c",LF);
    fprintf(fssock, "<INPUT TYPE=\"checkbox\" NAME=\"sub\" VALUE=\"3\">%s:%c",sublist[3],LF);
    fprintf(fssock, "Turkey breast topped with lettuce, mayo, alfalfa sprouts, and mayo.<P>%c",LF);
    fprintf(fssock, "<INPUT TYPE=\"checkbox\" NAME=\"sub\" VALUE=\"4\">%s:%c",sublist[4],LF);
    fprintf(fssock, "Genoa salami and provolone cheese topped with capacola, onion, lettuce, tomato, and Italian sauce.<P>%c",LF);
    fprintf(fssock, "<INPUT TYPE=\"checkbox\" NAME=\"sub\" VALUE=\"5\">%s:%c",sublist[5],LF);
    fprintf(fssock, "Layers of provolone cheese, separated by avocado, sprouts, lettuce, tomato, and mayo.<P>%c",LF);
    fprintf(fssock, "<INPUT TYPE=\"checkbox\" NAME=\"sub\" VALUE=\"6\">%s:%c",sublist[6],LF);
    fprintf(fssock, "1/4 pound of smoked ham, provolone cheese, topped with lettuce,%c",LF);
    fprintf(fssock, "tomato, and mayo.<P>%c",LF);
    fprintf(fssock, "<INPUT TYPE=\"checkbox\" NAME=\"sub\" VALUE=\"7\">%s:%c",sublist[7],LF);
    fprintf(fssock, "Shaved roast beef, provolone cheese, french dijon mustard, topped with shaved ham, lettuce,%c",LF);
    fprintf(fssock, "tomato, and mayo.<P>%c",LF);
    fprintf(fssock, "<INPUT TYPE=\"checkbox\" NAME=\"sub\" VALUE=\"8\">%s:%c",sublist[8],LF);
    fprintf(fssock, "Genoa salami, Italian capacola, smoked ham, and provolone cheese topped with lettuce,%c",LF);
    fprintf(fssock, "tomato, onions, mayo, and Italian sauce.<P>%c",LF);
    fprintf(fssock, "<INPUT TYPE=\"checkbox\" NAME=\"sub\" VALUE=\"9\">%s:%c",sublist[9],LF);
    fprintf(fssock, "1/4 pound of sliced roast beef, provolone cheese, topped with lettuce, tomato, and mayo.<P>%c",LF);
    fprintf(fssock, "<INPUT TYPE=\"checkbox\" NAME=\"sub\" VALUE=\"10\">%s:%c",sublist[10],LF);
    fprintf(fssock, "Turkey breast, smoked ham, and provolonecheese topped with lettuce, tomato, and mayo.<P>%c",LF);
    fprintf(fssock, "<INPUT TYPE=\"checkbox\" NAME=\"sub\" VALUE=\"11\">%s:%c",sublist[11],LF);
    fprintf(fssock, "Turkey breast, avocado, and cheese topped with lettuce, mayo, alfalfa, and tomato.<P>%c",LF);
    fprintf(fssock, "<H3>Slim Jim Subs</H3>%c",LF);
    fprintf(fssock, "Subs without veggies or sauce.<P>%c",LF);
    fprintf(fssock, "<INPUT TYPE=\"checkbox\" NAME=\"slj\" VALUE=\"0\">%s<P>%c",slimlist[0],LF);
    fprintf(fssock, "<INPUT TYPE=\"checkbox\" NAME=\"slj\" VALUE=\"1\">%s<P>%c",slimlist[1],LF);
    fprintf(fssock, "<INPUT TYPE=\"checkbox\" NAME=\"slj\" VALUE=\"2\">%s<P>%c",slimlist[2],LF);
    fprintf(fssock, "<INPUT TYPE=\"checkbox\" NAME=\"slj\" VALUE=\"3\">%s<P>%c",slimlist[3],LF);
    fprintf(fssock, "<INPUT TYPE=\"checkbox\" NAME=\"slj\" VALUE=\"4\">%s<P>%c",slimlist[4],LF);
    fprintf(fssock, "<INPUT TYPE=\"checkbox\" NAME=\"slj\" VALUE=\"5\">%s<P>%c",slimlist[5],LF);
    fprintf(fssock, "<H3>Side orders</H3>%c",LF);
    fprintf(fssock, "<INPUT TYPE=\"checkbox\" NAME=\"sde\" VALUE=\"0\">%s<P>%c",sidelist[0],LF);
    fprintf(fssock, "<INPUT TYPE=\"checkbox\" NAME=\"sde\" VALUE=\"1\">%s<P>%c",sidelist[1],LF);
    fprintf(fssock, "<H3>Drinks</H3>%c",LF);
    fprintf(fssock, "<INPUT TYPE=\"checkbox\" NAME=\"pop\" VALUE=\"0\">%s<P>%c",poplist[0],LF);
    fprintf(fssock, "<INPUT TYPE=\"checkbox\" NAME=\"pop\" VALUE=\"1\">%s<P>%c",poplist[1],LF);
    fprintf(fssock, "<INPUT TYPE=\"checkbox\" NAME=\"pop\" VALUE=\"2\">%s<P>%c",poplist[2],LF);
    fprintf(fssock, "<INPUT TYPE=\"checkbox\" NAME=\"pop\" VALUE=\"3\">%s<P>%c",poplist[3],LF);
    fprintf(fssock, "<H3>Your Address, Phone Number, and Name</H3>%c",LF);
    fprintf(fssock, "<INPUT TYPE=\"text\" NAME=\"name\" MAXLENGTH=\"32\">Name<P>%c",LF);
    fprintf(fssock, "<INPUT TYPE=\"text\" NAME=\"adr\" MAXLENGTH=\"64\">Address<P>%c",LF);
    fprintf(fssock, "<INPUT TYPE=\"text\" NAME=\"phone\" MAXLENGTH=\"10\">Phone Number<P>%c",LF);
    fprintf(fssock, "<INPUT type=\"submit\">%c",LF);
    fprintf(fssock, "</FORM>%c",LF);
}

void print_error(FILE* fssock, const char *reason) {
    fprintf(fssock, "<TITLE>Order Not Submitted</TITLE>%c",LF);
    fprintf(fssock, "<H1>Order Not Submitted</H1>%c",LF);
    fprintf(fssock, "Your order has not been submitted, because %s.<P>%c",reason,LF);
}

 void httprun(int ssock, const char * cl)  {
    register int x,m=0;
    char w[256];
    char tfile[L_tmpnam];
    int subs,slims,sides,drinks,allow;
    char name[32];
    char phone[10];
    char address[64];
    FILE *tfp,*order;
    FILE* fssock = fdopen(ssock, "w");

    fprintf(fssock, "Content-type: text/html%c%c",LF,LF);

    if((!cl) || (!cl[0])) {
        dump_form(fssock);
        return;
    }

    tmpnam(tfile);
    if(!(tfp=fopen(tfile,"w"))) {
        fprintf(fssock, "<TITLE>Server Error</TITLE>%c",LF);
        fprintf(fssock, "<H1>Server Error</H1>%c",LF);
        fprintf(fssock, "Server unable to get a temporary file. Please try again later.<P>%c",LF);
        return;
    }

    subs=0;slims=0;sides=0;drinks=0;allow=0;
    name[0]='\0';
    phone[0]='\0';
    address[0]='\0';

    for(x=0;cl[0] != '\0'; x++) {
        m=x;
        getword(w,cl,'=');
        plustospace(w);
        unescape_url(w);
        if(!strcmp(w,"pwd")) {
            getword(w,cl,'&');
            plustospace(w);
            unescape_url(w);
            allow=(strcmp(w,PASSWORD) ? 0 : 1);
        }
        if(!strcmp(w,"sub")) {
            getword(w,cl,'&');
            plustospace(w);
            unescape_url(w);
            subs |= (1 << atoi(w));
        }
        else if(!strcmp(w,"slj")) {
            getword(w,cl,'&');
            plustospace(w);
            unescape_url(w);
            slims |= (1 << atoi(w));
        }
        else if(!strcmp(w,"sde")) {
            getword(w,cl,'&');
            plustospace(w);
            unescape_url(w);
            sides |= (1 << atoi(w));
        }
        else if(!strcmp(w,"pop")) {
            getword(w,cl,'&');
            plustospace(w);
            unescape_url(w);
            drinks |= (1 << atoi(w));
        }
        else if(!strcmp(w,"name")) {
            getword(w,cl,'&');
            plustospace(w);
            unescape_url(w);
            strcpy(name,w);
        }
        else if(!strcmp(w,"phone")) {
            getword(w,cl,'&');
            plustospace(w);
            unescape_url(w);
            strcpy(phone,w);
        }
        else if(!strcmp(w,"adr")) {
            getword(w,cl,'&');
            plustospace(w);
            unescape_url(w);
            strcpy(address,w);
        }
    }

    if(!name[0]) {
        print_error(fssock, "you didn't give your name");
        return;
    }
    if(!address[0]){
        print_error(fssock, "you didn't give your address");
        return;
    }
    if(!phone[0]) {
        print_error(fssock, "you didn't give your phone number");
        return;
    }
    if((!subs) && (!slims) && (!sides) && (!drinks)) {
        print_error(fssock, "you didn't order anything");
        return;
    }

    if(allow) {
        char t[256];
        sprintf(t,"/bin/mail %s",JJ_FAX);
        if(!(order=popen(t,"w"))) {
            print_error(fssock, "the server was unable to open a pipe to mail");
            return;
        }
        fprintf(fssock, "<TITLE>Order Sent</TITLE>%c",LF);
        fprintf(fssock, "<H1>Order Sent</H1>%c",LF);
        fprintf(fssock, "Your order has been sent to the UIUC e-mail to FAX gateway.<P>%c",LF);
    } else {
        fprintf(fssock, "<TITLE>Your Order</TITLE>%c",LF);
        fprintf(fssock, "<H1>Your Order</H1>%c",LF);
        fprintf(fssock, "This is how your order would have looked if it had been sent.<P><PLAINTEXT>%c",LF);
        order=stdout;
    }

    fprintf(order,"My name is %s, and I would like to have the following%c",
            name,LF);
    fprintf(order,"order delivered to %s:%c%c",address,LF,LF);
    for(x=0;x<12;x++)
        if(subs & (1 << x))
            fprintf(order,"\t(1) %s%c",sublist[x],LF);
    for(x=0;x<6;x++)
        if(slims & (1 << x))
            fprintf(order,"\t(1) %s Slim Jim%c",slimlist[x],LF);
    for(x=0;x<2;x++)
        if(sides & (1 << x))
            fprintf(order,"\t(1) %s%c",sidelist[x],LF);
    for(x=0;x<4;x++)
        if(drinks & (1 << x))
            fprintf(order,"\t(1) %s%c",poplist[x],LF);
    fprintf(order,"%cPlease feel free to call me at %s if there is any%c",LF,
            phone,LF);
    fprintf(order,"problem. Thank you.%c%c.%c",LF,LF,LF);
    fclose(order);
    return;
}
