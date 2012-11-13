#!/bin/sh

source ./bashcolors.sh
initializeANSI

./attribute
./engine
./jwcmp
./ratios
./ratiocomponent
./blocking
./compare
./similarity
./assigneecomparison
./coauthor
./strcmp95
./stringmanipulator
./training
./clusterinfo
./cluster
./clusterhead
./fetchrecords
./misspell
./rarenames
./namecompare
./engineconfig



echo "Typedefs test"
`./typedefs`
echo "Abbreviations test"
`./abbreviation`
echo "Record test"
`./record`

source ./integration.sh

