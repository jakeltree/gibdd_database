pathh="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
echo "scene.sh was launched. pathh: ${pathh}"

cd $pathh/client1; bash scenario.sh &
cd $pathh/client2; bash scenario.sh &
cd $pathh/client3; bash scenario.sh &
cd $pathh/client4; bash scenario.sh &
