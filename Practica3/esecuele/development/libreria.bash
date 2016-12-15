COMMAND="./esecuele"

rm -rf libreria_db

$COMMAND createdb libreria_db

$COMMAND define libreria_db << EOF
TABLE edicion 7 STR STR STR STR STR STR STR
TABLE compra 5 STR LNG STR STR INT
TABLE usuario 5 LNG STR STR STR STR STR
EOF

$COMMAND insert libreria_db << EOF
COPY edicion ./example_files/edicion.txt
COPY compra ./example_files/compra.txt
COPY usuario ./example_files/usuario.txt
EOF

$COMMAND query libreria_db
