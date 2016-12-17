COMMAND="./esecuele"

rm -rf libreria_db

echo "Creando libreria"
$COMMAND createdb libreria_db

echo "Creando tablas"
$COMMAND define libreria_db << EOF
TABLE edicion 7 STR STR STR STR STR STR STR
TABLE compra 5 STR LNG STR STR INT
TABLE usuario 5 LNG STR STR STR STR STR
EOF

echo "Insertando datos en tablas"
$COMMAND insert libreria_db << EOF
COPY edicion ./example_files/edicion.txt
COPY compra ./example_files/compra.txt
COPY usuario ./example_files/usuario.txt
EOF

echo "Título de los libros comprados por jack:"
$COMMAND query libreria_db << EOF
usuario SEQUENTIAL 2 STR jack C_COLEQCTE SELECT compra SEQUENTIAL PRODUCT 0 6 C_COLEQCOL SELECT STR 5 P_COL 1 PROJECT edicion SEQUENTIAL PRODUCT 0 1 C_COLEQCOL SELECT STR 6 P_COL 1 PROJECT
EOF
echo  "Número de libros comprados por jack:"
$COMMAND query libreria_db << EOF
usuario SEQUENTIAL 2 STR jack C_COLEQCTE SELECT compra SEQUENTIAL PRODUCT 0 6 C_COLEQCOL SELECT COUNT
EOF
