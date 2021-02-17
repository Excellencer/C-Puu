/* Kalle Liljeström,  */
/* BM40A030 Tietorakenteet ja algoritmit */
/* AVL-puu harjoitustyö */
/* Lähteet: Luentokalvot, Helsingin yliopiston vastaavat luentodiat osoitteesta:
    https://www.cs.helsinki.fi/u/jkivinen/opetus/tira/k08/viikko04.pdf */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 30

typedef struct Alkio
{
    struct Alkio *vasen, *oikea;
    int avain;
    int pituus;
} Alkio;

Alkio *lueTD(Alkio *);
Alkio *lisaa(Alkio *, int);
void tulostus(Alkio *, int);
int haku(Alkio *, int);
int korkeus(Alkio *);
Alkio *oikeakierto(Alkio *);
Alkio *vasenkierto(Alkio *);
Alkio *RR(Alkio *);
Alkio *LL(Alkio *);
Alkio *LR(Alkio *);
Alkio *RL(Alkio *);
int painotus(Alkio *);
void vapautaMuisti(Alkio *);
/* Globaali muuttuja selostuksen muuttamiseksi ON/OFF */
int selostustGlobal = 0;

int main()
{
    Alkio *juuri = NULL;
    int x, i, valinta;

    printf("\nAVL-puu demo.\n");

    do
    {
        printf("\n1) Lue dataa tiedostosta");
        printf("\n2) Tulosta puu");
        printf("\n3) Haku");
        printf("\n4) Lisää alkio");
        printf("\n5) Selostus ON/OFF");
        printf("\n6) Tyhjennä puu");
        printf("\n7) Lopeta");
        printf("\n\nValitse toiminto: ");
        scanf("%d", &valinta);

        switch (valinta)
        {
        case 1:
            juuri = lueTD(juuri);
            break;

        case 2:
            tulostus(juuri, 0);
            break;

        case 3:
            printf("Haettava avain: ");
            scanf("%d", &x);
            i = haku(juuri, x);
            if (i == 1)
            {
                printf("%d löytyi.\n", x);
            }
            else
            {
                printf("%d ei löytynyt.\n", x);
            }
            break;

        case 4:
            printf("\nLisättävä avain: ");
            scanf("%d", &x);
            juuri = lisaa(juuri, x);
            break;

        case 5:
            if (selostustGlobal == 0)
            {
                selostustGlobal = 1;
                printf("Selostus ON\n");
            }
            else
            {
                selostustGlobal = 0;
                printf("Selostus OFF\n");
            }
            break;

        case 6:
            vapautaMuisti(juuri);
            juuri = NULL;
            break;
        }
    } while (valinta != 7);

    vapautaMuisti(juuri);

    return 0;
}

Alkio *lueTD(Alkio *juuri)
{
    char tiedostoNimi[MAX], data[MAX];
    int i = 0, x = 0;
    FILE *tiedosto;

    printf("Tidoston nimi: ");
    scanf("%s", tiedostoNimi);

    /* Virheentarkistus */
    if ((tiedosto = fopen(tiedostoNimi, "r")) == NULL)
    {
        printf("Tiedoston avaus epäonnistui.\n");
        exit(1);
    }

    while (fgets(data, MAX, tiedosto) != NULL)
    {
        data[strcspn(data, "\n")] = 0;

        if (strlen(data) == '\0')
        {
            printf("Tyhjä rivi, lopetetaan.\n");
            break;
        }
        x = atoi(data);
        juuri = lisaa(juuri, x);
        i++;
    }
    printf("Luettiin tiedosto '%s', %d avainta.\n", tiedostoNimi, i);

    fclose(tiedosto);

    return juuri;
}

/* Rekursiivinen alkion lisäys */
Alkio *lisaa(Alkio *solmu, int x)
{
    if (solmu == NULL)
    {   
        /* Muistinvaraus */
        solmu = (Alkio *)malloc(sizeof(Alkio));
        solmu->avain = x;
        solmu->vasen = NULL;
        solmu->oikea = NULL;
    }
    else if (x > solmu->avain) 
    {
        solmu->oikea = lisaa(solmu->oikea, x);
        /* Painotuksen tarkistus, jos oikea puu painavampi ->*/
        if (painotus(solmu) == -2)
        {
            if (x > solmu->oikea->avain)
            {
                solmu = LL(solmu);
            }
            else
            {
                solmu = RL(solmu);
            }
        }
    }
    else if (x < solmu->avain)
    {
        solmu->vasen = lisaa(solmu->vasen, x);
        /* Painotuksen tarkistus, jos vasen puu painavampi -> */
        if (painotus(solmu) == 2)
        {
            if (x < solmu->vasen->avain)
            {
                solmu = RR(solmu);
            }
            else
            {
                solmu = LR(solmu);
            }
        }
    }
    else
    /* Palataan, jos solmu on jo puussa */
    {
        return (solmu);
    }

    solmu->pituus = korkeus(solmu);

    return (solmu);
}

/* Tulostaa puun juurisolmu vasemmalla ja oikea alipuu esnsin */
void tulostus(struct Alkio *juuri, int taso)
{
    int i;

    if (juuri == NULL)
    {
        return;
    }
    /* Kasvatetaan tasoa, jokaisella läpikäynnillä */
    taso++;

    tulostus(juuri->oikea, taso);

    printf("\n");

    /* Tabulaattoreita solmun korkeuden mukaan */
    for (i = 1; i < taso; i++)
    {
        printf("\t");
    }
    printf("%d(%d)\n", juuri->avain, juuri->pituus);

    tulostus(juuri->vasen, taso);
}

/* Rekursiivinen haku, palauttaa 1 tai 0, riippuen löytyykö alkiota */
int haku(Alkio *juuri, int x)
{
    if (juuri == NULL)
    {
        return 0;
    }

    if (juuri->avain == x)
    {
        return 1;
    }
    else if (juuri->avain < x)
    {
        return haku(juuri->oikea, x);
    }
    else
    {
        return haku(juuri->vasen, x);
    }
}

/* Laskee solmun korkeuden lehtipuista, lehtipuun korkeus 0 */
int korkeus(Alkio *solmu)
{
    int vp, op;
    if (solmu == NULL)
    {
        return (0);
    }

    if (solmu->vasen == NULL)
    {
        vp = 0;
    }
    else
    {
        vp = 1 + solmu->vasen->pituus;
    }

    if (solmu->oikea == NULL)
    {
        op = 0;
    }
    else
    {
        op = 1 + solmu->oikea->pituus;
    }
    /* Verrataan kumpi alipuu syvempi ja valitaan se */
    if (vp > op)
    {
        return (vp);
    }
    return (op);
}

Alkio *oikeakierto(Alkio *a)
{
    Alkio *b;
    b = a->vasen;
    a->vasen = b->oikea;
    b->oikea = a;

    a->pituus = korkeus(a);
    b->pituus = korkeus(b);
    return (b);
}

Alkio *vasenkierto(Alkio *a)
{
    Alkio *b;
    b = a->oikea;
    a->oikea = b->vasen;
    b->vasen = a;

    a->pituus = korkeus(a);
    b->pituus = korkeus(b);
    return (b);
}

Alkio *LL(Alkio *solmu)
{
    solmu = vasenkierto(solmu);
    if (selostustGlobal == 1)
    {
        printf("Kierretään '%d' LL.\n", solmu->avain);
    }
    return (solmu);
}

Alkio *RR(Alkio *solmu)
{
    solmu = oikeakierto(solmu);
    if (selostustGlobal == 1)
    {
        printf("Kierretään '%d' RR.\n", solmu->avain);
    }
    return (solmu);
}

Alkio *LR(Alkio *solmu)
{
    solmu->vasen = vasenkierto(solmu->vasen);
    solmu = oikeakierto(solmu);

    if (selostustGlobal == 1)
    {
        printf("Kierretään '%d' LR.\n", solmu->avain);
    }
    return (solmu);
}

Alkio *RL(Alkio *solmu)
{
    solmu->oikea = oikeakierto(solmu->oikea);
    solmu = vasenkierto(solmu);

    if (selostustGlobal == 1)
    {
        printf("Kierretään '%d' RL.\n", solmu->avain);
    }
    return (solmu);
}

/* Laskee onko vasen vai oikea alipuu painavampi. Palauttaa -merkkisen, jos oikea painavampi */
int painotus(Alkio *solmu)
{
    int vp, op;
    if (solmu == NULL)
    {
        return (0);
    }
    if (solmu->vasen == NULL)
    {
        vp = 0;
    }
    else
    {
        vp = 1 + solmu->vasen->pituus;
    }
    if (solmu->oikea == NULL)
    {
        op = 0;
    }
    else
    {
        op = 1 + solmu->oikea->pituus;
    }

    return (vp - op);
}

/* Muistinvapautus rekursiivisesti*/
void vapautaMuisti(Alkio *solmu)
{
    if (solmu == NULL)
    {
        return;
    }
    vapautaMuisti(solmu->vasen);
    vapautaMuisti(solmu->oikea);
    free(solmu);
    return;
}