COMMAND="./esecuele"

rm -rf cine

$COMMAND createdb cine

$COMMAND define cine << EOF
TABLE pelis 4 INT STR STR DBL
TABLE movies 4 INT STR STR DBL
EOF

$COMMAND insert cine << EOF
COPY pelis ./example_files/peliculas.txt
COPY movies ./example_files/movies.txt
EOF

$COMMAND query cine << EOF
movies SEQUENTIAL
EOF

$COMMAND query cine << EOF
pelis SEQUENTIAL
EOF

$COMMAND query cine << EOF
movies SEQUENTIAL 3 LIMIT
EOF

$COMMAND query cine << EOF
pelis SEQUENTIAL 2 OFFSET
EOF

$COMMAND query cine << EOF
pelis SEQUENTIAL COUNT
EOF

$COMMAND query cine << EOF
pelis SEQUENTIAL movies SEQUENTIAL UNION
EOF
