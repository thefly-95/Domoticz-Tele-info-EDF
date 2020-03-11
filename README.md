# Domoticz-Tele-info-EDF
Remonté de la télé info du compteur EDF Linky vers Domoticz

Je suis parti de la librairie TeleInfo à l’Arduino IDE disponible ici : https://github.com/sremy91/LibTeleinfo
Toutes les instrucitions sont ici https://anderson69s.com/2018/05/01/domoticz-linky/

J'ai effectué quelques changement dans le fichier webclient.cpp pour utiliser le login/mot de passe de mon serveur Domoticz à savoir :
1. Configuration avec login/password:

    basicauthusr = config.domoticz.usr;
    basicauthpwd = config.domoticz.pwd;
    
2. Changé le premier champ dans l'onglet config pour pouvoir accueillir un switch sur HeureCreuse/HeurePleine:
//json.htm?type=command&param=switchlight&idx=99&switchcmd=On
    
3. Changé l'ordonnancement de l'écriture // HCHP,HCHC,0,0,PAPP,0
   // /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=USAGE1;USAGE2;RETURN1;RETURN2;CONS;PROD
  
  Corrigé les commentaires de l'onglet config dans le ficheir index.html.gz
  
  Et voilou, tous fonctionne avec le serveur Domoticz avec mot de passe.
  
