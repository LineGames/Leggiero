﻿////////////////////////////////////////////////////////////////////////////////
// Sugar/SugarHeart.h (Leggiero - Utility)
//
// Minnani Todoke Shyuuga-ha-to☆
////////////////////////////////////////////////////////////////////////////////

#ifndef __UTILITY__SUGAR__SUGAR_HEART_H
#define __UTILITY__SUGAR__SUGAR_HEART_H

/*
                                  ::.
                                .==:.
                                :+.
                            .:::=+
                          .-=====--:-::                ::::-::.
                        .===-:--==*=.:-:            :==-:.:::-==:
                       -++-:-==++=+=.::-           =*=.        -*=
                     .=+=:-=****-:-+-..-:...:::.  +*:           .+*.
                    .++::-+*+-**---=+-:+***+====:=*.              =*.
                   :*=::-*+--:*#=--=*++*+==-----=+=::.             =+
                  :*=-:-*=-:-:+#*++**++=------------==-     .       +-
                 -*=-:-+-::----*%##*-==---------------=-::: .-.     .=.
                :#+---=-:::--:=#%*==+------==-----------+-:: -:      :-
               :*+---==--:--:-#**=-==----:==-------------+-:-=-       -.
              .**-------:--::*#=+*+-:---:-+------------=-=+=*+.       ::
              +*=------::-::=#*=-*=:-----+---:---------=---=+=-       .-
             =*+----------:-**+=-=:----:+=:-:----:-----=----==+-       -.
            :**=----------:+%=+==-----.=+:--:=--::-::--=-----+*=:      --
           .*++------------##==+=----::+---:==--:--:---------=%+=.     ::
           =*+=----------:+%*+-+-----:=+--:-=----=--=-------=-**+-     .:
          :*++=-----------*#++=-------+----+----=-:+-----:--=-=**=:    :-
          ++++---=------:=##*++-----:++--=+---=+-:+=-=---:-----*++-    ::
         -*+*=---=-------*##*++------*-:=+-:-++-:++-+---------:++++.   ::
         +++*=--==------=###*+=----:=*-+*=-=++--++-+-----:==--:=*+=-   :
        :*+**=--==-------*#**+------*+++-=**+:-=+-*=:-----+-=-:=*===.
        =*+*+=--+--------*#**+----:=*++====-===+-=+:---=:*+-=::-*+==.
       .++**+--=+-------:**+*+:---:+*++++=--=+=:-=----=--*-=--:-++=-:
       :*+**+--=+------:.#*++=-----++=+=::::=:.:=--=-==-=--*----+*=--
       -*+**+--++-=---=. **+*----:=-:#@%*-..  -=-=+-=++-:=--=---+*=--.
       =*+**+--++-=---=: *#*+------#%+%@##= -+--=+-=++=-== -=---+*+--.
       +++#++-:++-=---=: =#==-----#%=-#@*:+ .:=++=+*=:.*+  -==--+++--:
       +++#++=:++-+---=. -+-+-----#:*@@@# ..  -==+=: :+-...-==--+++--:
      .++*#++=:=*-+=--=   :++------:@@@@+ ....  .   :-. ..:=+---+++---
      .*+**+++-=*=+=--+=. .+=---=-::%%@%  ....... -#+  .. :++---+++=--
       *+***++=-*=++-:*%*..+=--==-: -*#: .......  =@@*  . -*=---+++=--
       +*+#*+++-++++-:*%*+:===-==--. :: ........ :*%@@+  .**----+++=--.
       +#+**++*==*++=:+%**--==-==--... ......... *@@@*#  +#+-=--+++=--.
       -#+**++++=++++=+#++--+=:==--............ .%@@@-*::%*=-=:-+++=--.
       .#*+#*+++==+++++*+*--+=:+==-............ :%%@# #:=#+-==:=*++=--.
        =%+**++++=+++**+*%:-+=:+===..............=##.-=-%*==+-:=*++=--
         ##*#+++++=+++**#* -++-====.............. -. ::%#+==+--++++=--.
         :###*+++++++++*#: :++-=+-=: ............. .  #%++=+=:=*+++==-.
          :*#%**++++++++*: =*+==+:+- ............... =@*=+=+-:+*+++==-
           .=*#**+++++++*. +*++=+=-- . :-    ...... :%#=+=+=:-*++++==-
              =%#*+++++#+ .--*+=++:-- . :..-: ..... +%=++++-:=*++++==-
               =#%#***##. -- *++++=::......:. .... -%+=+++-:-*+++++=-:
                :+####+.  =: -*++*+=. ....  .:    -*+=+++=:-+#+++++--.
                  .::.    =.  =*++*++: ..... -+::-+++**+=::+**+++++--.
                          -    -****#+. .   ..=**+++===+--+**++++*=-=
                         .-     :+==--=:.:-=-: .+*-.   .+#**++++*=:=:
                         .-     .:::::-=--=::-.==.  . ..-+*++++**-=-
                         .-    :.:-:::::::  ..-*  .  :--:.=*+++*==+.
                         .-   .: .-::::..    -*. .  -=::::-+++#*+*:
                          :  :-..  :....     *= .  --::--==+#%%#*:
                        ..-::-:..  .:.      :+. . --::-=-:.-#*+-
                       :::==:::.  :=-==     =- . ----=+-.   :
                      ::. --.--   =-:-#=   .=:  :=:-+*-  ..  ..:...
                     .:.  -- :-. :=:::++:  :=: .-::=#+. .... :.  ....
        ...         -+... .+. .::.-=-+= ::..-. --::**:.........    .....
        ....        *+ ... =+  ...=+**=  .. =::-::-*-::...... ..     .......
      ...-. .       =+.... .*= ..=-===- ... .==:::==:::. .......         .:::.
      ::..::....    :*: ... .**-==:=:=- ...  --:::=---:..........            ..
      .-:..::....    *+  ... .-:+-:-:=: ..  --:::-=----:.........            ..
       --:...:. ...  =*- :...  -=:---=: .. --::::-+---=-..........           ..
       :-:...::..... -*+-:: ...+  =: -- . :-:::::=+=---=:..........         ..
       .-::...::   .. +--=. ...=-.:..=-  :=:::::-+==-::-=:............    ...
       .=:.----::::.. -*.=:  . :=---=-. -+-::::-==-=-::.+=...........::....
        :-:---:::... .-#=.--. ..--:=: .:=-==---=+-:==:::=--......... .:
        :=:::.     . .=++::--:  :--:..--::-==-===:--=:::= -:...... :...
         :-:::........:=*::::-=:.:::---::::--:-=::--=-.-+..-:.......::
          .::::-.....:..+=::::==-:-=--::::::--:..----=.-+. .-...... ..
             .---=---: .:+-::-=---++-::::::::==:.-----=+=::.--...... ..
             .:.-::::  :.=+:-*=::::-=--::::::-=-:------+- .::-:.......:.
             .-::.    .:..++==-::::::--=-::--=-:---------     :::........
              ::::=-  .:..:=:--::::::::--=--=-:--------=-     .-::........
              .:::+*.  -:.. :=:::::::::::-==-:----=----=-      :::.........
                ::::.. --...-=-:::::::::::-=-----+=--:-=-       :::.........
                  :. . .=-:.-+=-::::::::::::==--++---.:=-       .::..........
                  .:.. .==---=++::-::::::::::-==+---...--        :::.........
                    .:--==----=++---::::::::::=+---:...:-        .:::.........
*/

#endif