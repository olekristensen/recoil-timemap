# timemap #3 #

software for a dance performance
_recoil performance group, spring 2009_

an interactive video scenography for a recoil performance group dance performance/installation. developed with choreographer tina tarpgaard and programmers ole kristensen and jonas jongejan.

http://olekristensen.files.wordpress.com/2009/08/timemap3_forestilling_dok-8596.jpg?h=250
http://olekristensen.files.wordpress.com/2009/08/timemap3presse-7208.jpg?h=250

### Description ###

time map is a dance performance that is sampled, manipulated and reconstructed, as if you where in a film set and part of a live editing process.
in this time displaced universe 3 people meet. ottilia 1886, rose 2008, keem 2258 – each of them seeking to find justice.
the three dancers have worked from a base of true stories from past, present and future.
being placed in past, present and future their stories are portrayed visually, physically and dramatically.
they meet in the court room in a battle with words – from taking orders to making them fly across the screens.

### Technology ###

ole kristensen and jonas jongejan created the videoscenography using infrared motion tracking implemented in openframeworks. all video and lights were cued from qlab using midi to isadora through to a grandma node, our own custom osc server, motion capture and opengl renderer. during the performance all text was typed live by the author gritt uldall on stage. the choreographer had control over some of the scenes from her own laptop. thus further osc controlpanels for text editing and choreographer interaction were made using maxmsp. some parts of the first act were filmed and edited on stage during second act for an epilogue shown in the lobby.

### Code Disclaimer ###

while the code is full of all kinds of nice trickeries, the applications themselves are not of much use outside the scope of our performance. it will probably not run on your machine, first of all it needs a collection of more or less obscure hardware, secondly it works tightly together with other third party apps such as isadora, maxmsp, grabberraster, qlab and even a pc-based grandma lighting node - and i haven't even listed the c++ lib dependencies

... and the code is messy too - so read if you please, compile if you can - but don't expect it to compute for you