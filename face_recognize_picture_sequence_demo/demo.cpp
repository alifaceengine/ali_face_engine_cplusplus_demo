#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>

#include "FaceEngine.h"
#include "../common.h"

using namespace ali_face_engine;
using namespace std;

#define TAG "FaceRecognizeMP4Demo"

static string PICTURE_ROOT = "../pictures/";

FaceRegister *sFaceRegister;
FaceRecognize *sFaceRecognize;
FaceDetect *sFaceDetect;

//persons, jpgfile : personname_featureid.jpg
#if 0
static const char *BASE_PERSONS[] = {
        "朱娣.jpg", "卢锦均.jpg", "徐凯林.jpg", "沈丽华.jpg", "郝凯凯.jpg", "李照.jpg", "吴春新.jpg", "徐晨潇.jpg", "沈冬英.jpg", "金锦虹.jpg",
        "沈蓉.jpg", "吴润芝.jpg", "方建梁.jpg", "沈晓英.jpg", "钟芳玲.jpg", "潘红.jpg", "周石英.jpg", "朱佳杰.jpg", "沈聪英.jpg", "阮文涛.jpg", "王卓.jpg",
        "周美芳.jpg", "朱郝涛.jpg", "洪国斌.jpg", "陈丹妮.jpg", "王浩.jpg", "唐金来.jpg", "朱静怡.jpg", "王利华.jpg", "陈佳凤.jpg", "甄杰.jpg", "姚求英.jpg",
        "李云奇.jpg", "王喜文.jpg", "陈彩珠.jpg", "胡寒.jpg", "孟显召.jpg", "李亚红.jpg", "王孝全.jpg", "陈得泉.jpg", "苏钊.jpg", "宋星星.jpg", "李占国.jpg",
        "王玉云.jpg", "陈有珠.jpg", "于杰.jpg", "赵磊.jpg", "封小珍.jpg", "李怀勇.jpg", "窦文松.jpg", "陈正阳.jpg", "卢政.jpg", "郑建.jpg", "崔加高.jpg",
        "李树平.jpg", "管明燕.jpg", "韩义国.jpg", "周晴.jpg", "陆强.jpg", "廖柳樱.jpg", "李英子.jpg", "翁海潮.jpg", "马敏霞.jpg", "宋亮.jpg", "仲亚婷.jpg",
        "张建义.jpg", "杨志成.jpg", "苏孝青.jpg", "高德祥.jpg", "张桢.jpg", "俞嘉琛.jpg", "张承余.jpg", "林开杰.jpg", "蒋挺飞.jpg", "魏月英.jpg", "彭静.jpg",
        "倪王根.jpg", "张新兰.jpg", "樊国华.jpg", "谢丽霞.jpg", "方敏.jpg", "凌潜兴.jpg", "张明大.jpg", "武佳楠.jpg", "贾伟斌.jpg", "曹敏.jpg", "刘伟红.jpg",
        "张洪斌.jpg", "毛海燕.jpg", "邰洋一.jpg", "曾咏.jpg", "刘星驰.jpg", "张红兵.jpg", "汪斌权.jpg", "邵俊波.jpg"};
#else
static const char *BASE_PERSONS[] = {
        "63.jpg", "823.jpg", "1409.jpg", "189.jpg", "77.jpg", "837.jpg", "1353.jpg", "1435.jpg", "1421.jpg", "1347.jpg",
        "1192.png", "638.jpg", "1390.jpg", "604.jpg", "162.jpg", "176.jpg", "88.jpg", "1384.jpg", "1179.jpg",
        "1637.jpg", "1151.jpg", "1145.jpg", "1623.jpg", "348.jpg", "360.jpg", "406.jpg", "1186.jpg", "412.jpg",
        "374.jpg", "1019.jpg", "599.jpg", "1743.jpg", "1025.jpg", "764.png", "1031.jpg", "1757.jpg", "228.jpg",
        "214.jpg", "1780.jpg", "572.jpg", "566.jpg", "200.jpg", "957.jpg", "1569.jpg", "943.jpg", "1227.jpg",
        "1541.jpg", "1555.jpg", "1233.jpg", "994.jpg", "758.jpg", "980.jpg", "770.jpg", "1582.jpg", "1596.jpg",
        "1597.jpg", "765.jpg", "1030.png", "771.jpg", "1583.jpg", "981.jpg", "759.jpg", "995.jpg", "1554.jpg",
        "1232.jpg", "1226.jpg", "1540.jpg", "1568.jpg", "942.jpg", "956.jpg", "567.jpg", "201.jpg", "1781.jpg",
        "215.jpg", "573.jpg", "229.jpg", "1756.jpg", "1742.jpg", "1024.jpg", "598.jpg", "1018.jpg", "413.jpg",
        "1187.jpg", "375.jpg", "361.jpg", "1193.jpg", "407.jpg", "349.jpg", "836.png", "1144.jpg", "1622.jpg",
        "1636.jpg", "1150.jpg", "1178.jpg", "177.jpg", "1385.jpg", "611.jpg", "89.jpg", "1391.jpg", "163.jpg",
        "639.jpg", "1420.jpg", "1346.jpg", "1352.jpg", "1434.jpg", "188.jpg", "76.jpg", "822.jpg", "1408.jpg", "74.jpg",
        "1378.jpg", "834.jpg", "60.jpg", "820.jpg", "48.jpg", "1422.jpg", "1436.jpg", "1350.jpg", "149.jpg", "613.jpg",
        "1387.jpg", "161.jpg", "1393.jpg", "607.jpg", "388.jpg", "1608.jpg", "1620.jpg", "1146.jpg", "1152.jpg",
        "1634.jpg", "439.jpg", "377.jpg", "1185.jpg", "411.jpg", "808.png", "1344.png", "405.jpg", "1191.jpg",
        "363.jpg", "1768.jpg", "1754.jpg", "1026.jpg", "559.jpg", "203.jpg", "565.jpg", "571.jpg", "217.jpg",
        "1556.png", "940.jpg", "798.jpg", "954.jpg", "1218.jpg", "1230.jpg", "1542.jpg", "968.jpg", "1224.jpg",
        "983.jpg", "997.jpg", "767.jpg", "1581.jpg", "773.jpg", "1032.png", "1580.jpg", "772.jpg", "766.jpg",
        "1594.jpg", "996.jpg", "982.jpg", "1543.jpg", "1225.jpg", "969.jpg", "1231.jpg", "1557.jpg", "1219.jpg",
        "955.jpg", "799.jpg", "941.jpg", "570.jpg", "216.jpg", "202.jpg", "564.jpg", "558.jpg", "1027.jpg", "1741.jpg",
        "1755.jpg", "1033.jpg", "1769.jpg", "1190.jpg", "404.jpg", "49.png", "362.jpg", "376.jpg", "410.jpg",
        "1184.jpg", "438.jpg", "1153.jpg", "1635.jpg", "1621.jpg", "1147.jpg", "1609.jpg", "160.jpg", "606.jpg",
        "1392.jpg", "1386.jpg", "612.jpg", "174.jpg", "389.png", "148.jpg", "1351.jpg", "809.jpg", "1345.jpg",
        "1423.jpg", "821.jpg", "61.jpg", "835.jpg", "1379.jpg", "75.jpg", "1427.jpg", "414.png", "1355.jpg", "59.jpg",
        "819.jpg", "1433.jpg", "71.jpg", "831.jpg", "1369.jpg", "65.jpg", "825.jpg", "616.jpg", "1382.jpg", "1396.jpg",
        "602.jpg", "164.jpg", "158.jpg", "1143.jpg", "1625.jpg", "170.png", "1631.jpg", "1157.jpg", "1619.jpg",
        "399.jpg", "1180.jpg", "372.jpg", "366.jpg", "1341.png", "400.jpg", "1194.jpg", "428.jpg", "1037.jpg",
        "1751.jpg", "1745.jpg", "1023.jpg", "1779.jpg", "560.jpg", "1221.png", "206.jpg", "212.jpg", "979.png",
        "574.jpg", "548.jpg", "1553.jpg", "1235.jpg", "1547.jpg", "945.jpg", "1209.jpg", "951.jpg", "789.jpg",
        "1590.jpg", "776.jpg", "1584.jpg", "986.jpg", "992.jpg", "993.jpg", "987.jpg", "777.jpg", "1591.jpg", "763.jpg",
        "950.jpg", "1208.jpg", "944.jpg", "1220.jpg", "1546.jpg", "1552.jpg", "1234.jpg", "549.jpg", "213.jpg",
        "575.jpg", "978.png", "561.jpg", "207.jpg", "1778.jpg", "1744.jpg", "1022.jpg", "1036.jpg", "1750.jpg",
        "429.jpg", "367.jpg", "1195.jpg", "401.jpg", "415.jpg", "1181.jpg", "373.jpg", "398.jpg", "1618.jpg",
        "1630.jpg", "1156.jpg", "1142.jpg", "1624.jpg", "159.jpg", "603.jpg", "1397.jpg", "165.jpg", "171.jpg",
        "1383.jpg", "617.jpg", "824.jpg", "64.jpg", "1368.jpg", "830.jpg", "70.jpg", "818.jpg", "58.jpg", "1354.jpg",
        "1432.jpg", "1426.jpg", "1340.jpg", "1430.jpg", "8.jpg", "417.png", "1356.jpg", "1342.jpg", "198.jpg", "66.jpg",
        "826.jpg", "72.jpg", "832.jpg", "1418.jpg", "167.jpg", "1395.jpg", "601.jpg", "99.jpg", "615.jpg", "1381.jpg",
        "173.jpg", "629.jpg", "1154.jpg", "1632.jpg", "1626.jpg", "1140.jpg", "629.png", "1168.jpg", "403.jpg",
        "1197.jpg", "365.jpg", "371.jpg", "1183.jpg", "359.jpg", "1020.jpg", "1746.jpg", "1752.jpg", "1034.jpg",
        "588.jpg", "1008.jpg", "577.jpg", "211.jpg", "205.jpg", "563.jpg", "1222.png", "946.png", "239.jpg", "1544.jpg",
        "1236.jpg", "1550.jpg", "1578.jpg", "952.jpg", "1587.jpg", "761.jpg", "1593.jpg", "749.jpg", "991.jpg",
        "589.png", "984.jpg", "990.jpg", "748.jpg", "760.jpg", "1592.jpg", "1586.jpg", "774.jpg", "1579.jpg", "576.png",
        "1237.jpg", "1551.jpg", "1545.jpg", "1223.jpg", "562.png", "238.jpg", "953.png", "947.png", "204.jpg",
        "210.jpg", "1009.jpg", "1753.jpg", "1035.jpg", "1021.jpg", "1747.jpg", "358.jpg", "370.jpg", "416.jpg",
        "1182.jpg", "1196.jpg", "402.jpg", "364.jpg", "1169.jpg", "1627.jpg", "1141.jpg", "614.png", "628.jpg",
        "1380.jpg", "1155.png", "1633.png", "172.jpg", "166.jpg", "98.jpg", "600.jpg", "1394.jpg", "833.jpg", "73.jpg",
        "1419.jpg", "199.jpg", "827.jpg", "67.jpg", "1343.jpg", "1425.jpg", "9.jpg", "1431.jpg", "1357.jpg", "698.jpg",
        "840.jpg", "14.jpg", "1318.jpg", "854.jpg", "459.png", "1456.jpg", "1330.jpg", "471.png", "28.jpg", "1324.jpg",
        "868.jpg", "883.jpg", "897.jpg", "129.jpg", "1495.jpg", "101.jpg", "1126.png", "667.jpg", "673.jpg", "115.jpg",
        "1481.jpg", "1668.jpg", "1132.jpg", "1654.jpg", "1640.jpg", "465.jpg", "1442.png", "303.jpg", "1697.jpg",
        "1683.jpg", "317.jpg", "288.jpg", "1708.jpg", "1046.jpg", "1720.jpg", "1734.jpg", "1052.jpg", "539.jpg",
        "511.jpg", "1085.jpg", "277.jpg", "263.jpg", "1091.jpg", "934.jpg", "1278.jpg", "920.jpg", "1522.jpg",
        "1244.jpg", "1250.jpg", "1536.jpg", "1287.jpg", "713.jpg", "707.jpg", "1293.jpg", "1047.png", "1292.jpg",
        "706.jpg", "712.jpg", "1286.jpg", "1251.jpg", "276.png", "1537.jpg", "1523.jpg", "1245.jpg", "909.jpg",
        "921.jpg", "1279.jpg", "262.jpg", "504.jpg", "1090.jpg", "1084.jpg", "510.jpg", "935.png", "538.jpg",
        "1735.jpg", "1053.jpg", "1721.jpg", "1709.jpg", "289.jpg", "316.jpg", "1682.jpg", "470.jpg", "464.jpg",
        "1696.jpg", "302.jpg", "458.jpg", "1641.jpg", "1669.jpg", "672.jpg", "1480.jpg", "114.jpg", "1655.png",
        "100.jpg", "1494.jpg", "666.jpg", "1127.png", "896.jpg", "128.jpg", "882.jpg", "869.jpg", "1325.jpg", "29.jpg",
        "1443.jpg", "1457.jpg", "1331.jpg", "855.jpg", "1319.jpg", "15.jpg", "841.jpg", "699.jpg", "17.jpg", "857.jpg",
        "843.jpg", "1441.jpg", "1327.jpg", "1455.jpg", "894.jpg", "880.jpg", "658.jpg", "116.jpg", "1482.jpg",
        "670.jpg", "664.jpg", "1496.jpg", "102.jpg", "1119.jpg", "499.jpg", "1125.jpg", "1643.jpg", "1657.jpg",
        "1131.jpg", "1469.png", "328.jpg", "857.png", "1333.png", "472.jpg", "1680.jpg", "300.jpg", "1694.jpg",
        "466.jpg", "1079.jpg", "1051.jpg", "1737.jpg", "1723.jpg", "1045.jpg", "248.jpg", "1092.jpg", "506.jpg",
        "260.jpg", "274.jpg", "512.jpg", "1086.jpg", "1509.jpg", "937.jpg", "1535.jpg", "1253.jpg", "1247.jpg",
        "738.jpg", "704.jpg", "1290.jpg", "1284.jpg", "711.jpg", "1285.jpg", "1291.jpg", "705.jpg", "739.jpg",
        "1246.jpg", "1520.jpg", "1534.jpg", "1252.jpg", "1087.png", "936.jpg", "1508.jpg", "922.jpg", "275.jpg",
        "513.jpg", "507.jpg", "1093.jpg", "261.jpg", "249.jpg", "1722.jpg", "1044.jpg", "1050.jpg", "1736.jpg",
        "1078.jpg", "1695.jpg", "301.jpg", "467.jpg", "473.jpg", "1332.png", "315.jpg", "1681.jpg", "329.jpg",
        "1656.jpg", "1130.jpg", "1642.jpg", "498.jpg", "1118.jpg", "665.jpg", "1124.png", "103.jpg", "1497.jpg",
        "1483.jpg", "117.jpg", "671.jpg", "659.jpg", "881.jpg", "895.jpg", "1454.jpg", "1440.jpg", "1326.jpg",
        "842.jpg", "1468.jpg", "856.jpg", "16.jpg", "1322.jpg", "1444.jpg", "1450.jpg", "1336.jpg", "12.jpg", "852.jpg",
        "1478.jpg", "846.jpg", "675.jpg", "113.jpg", "1487.jpg", "1493.jpg", "661.jpg", "891.jpg", "649.jpg", "885.jpg",
        "107.png", "1646.jpg", "1120.jpg", "1134.jpg", "1652.jpg", "488.jpg", "1108.jpg", "1685.jpg", "311.jpg",
        "477.jpg", "463.jpg", "1691.jpg", "339.jpg", "1732.jpg", "1054.jpg", "1040.jpg", "1726.jpg", "1068.jpg",
        "265.jpg", "1097.jpg", "503.jpg", "517.jpg", "271.jpg", "259.jpg", "1083.png", "1256.jpg", "1530.jpg",
        "1524.jpg", "1242.jpg", "926.jpg", "1518.jpg", "932.jpg", "701.jpg", "1295.jpg", "1281.jpg", "715.jpg",
        "729.jpg", "728.jpg", "714.jpg", "1280.jpg", "700.jpg", "1519.jpg", "933.jpg", "927.jpg", "1525.jpg",
        "1243.jpg", "1257.jpg", "1531.jpg", "258.jpg", "1082.jpg", "516.jpg", "270.jpg", "1525.png", "264.jpg",
        "502.jpg", "1096.jpg", "1069.jpg", "1041.jpg", "1294.png", "1727.jpg", "1733.jpg", "1280.png", "1055.jpg",
        "338.jpg", "462.jpg", "1690.jpg", "304.jpg", "310.jpg", "1684.jpg", "476.jpg", "1109.jpg", "489.jpg",
        "1135.jpg", "1647.jpg", "1121.jpg", "884.jpg", "648.jpg", "890.jpg", "106.jpg", "1492.jpg", "660.jpg",
        "674.jpg", "1486.jpg", "112.jpg", "1653.png", "847.jpg", "853.jpg", "13.jpg", "1479.jpg", "1451.jpg",
        "1337.jpg", "1323.jpg", "1445.jpg", "1335.jpg", "39.jpg", "474.png", "1447.jpg", "1309.jpg", "845.jpg",
        "689.jpg", "11.jpg", "851.jpg", "662.jpg", "1490.jpg", "104.jpg", "110.jpg", "676.jpg", "886.jpg", "138.jpg",
        "892.jpg", "1651.jpg", "1137.jpg", "1123.jpg", "1645.jpg", "1679.jpg", "306.jpg", "1692.jpg", "460.jpg",
        "1321.png", "879.png", "1686.jpg", "312.jpg", "1453.png", "448.jpg", "1725.jpg", "1043.jpg", "1057.jpg",
        "1731.jpg", "1719.jpg", "299.jpg", "272.jpg", "514.jpg", "1080.jpg", "1094.jpg", "500.jpg", "266.jpg",
        "528.jpg", "1241.jpg", "1527.jpg", "1533.jpg", "919.jpg", "1255.jpg", "931.jpg", "925.jpg", "1269.jpg",
        "1282.jpg", "716.jpg", "702.jpg", "1296.jpg", "1297.jpg", "703.jpg", "717.jpg", "1283.jpg", "1268.jpg",
        "924.jpg", "930.jpg", "1532.jpg", "1254.jpg", "918.jpg", "1240.jpg", "1526.jpg", "529.jpg", "501.jpg",
        "1095.jpg", "267.jpg", "273.jpg", "1081.jpg", "515.jpg", "298.jpg", "1718.jpg", "1056.jpg", "1730.jpg",
        "1724.jpg", "1042.jpg", "449.jpg", "475.jpg", "313.jpg", "1687.jpg", "1693.jpg", "307.jpg", "461.jpg",
        "1678.jpg", "1122.jpg", "1644.jpg", "1650.jpg", "1136.jpg", "893.jpg", "887.jpg", "139.jpg", "1485.jpg",
        "111.jpg", "677.jpg", "663.jpg", "105.jpg", "1491.jpg", "850.jpg", "10.jpg", "688.jpg", "844.jpg", "1308.jpg",
        "1446.jpg", "1320.jpg", "878.jpg", "38.jpg", "1334.jpg", "1452.jpg", "21.jpg", "861.jpg", "35.jpg", "1339.jpg",
        "875.jpg", "1477.jpg", "1311.jpg", "1305.jpg", "691.jpg", "444.png", "1463.jpg", "1488.jpg", "108.jpg",
        "1661.png", "646.jpg", "652.jpg", "134.jpg", "1649.jpg", "1113.jpg", "487.jpg", "1675.jpg", "493.jpg",
        "1107.jpg", "478.jpg", "322.jpg", "336.jpg", "450.jpg", "685.png", "1729.jpg", "1067.jpg", "1701.jpg",
        "295.jpg", "281.jpg", "1715.jpg", "1073.jpg", "1098.jpg", "518.jpg", "256.jpg", "242.jpg", "524.jpg", "915.jpg",
        "1259.jpg", "901.jpg", "1503.jpg", "929.jpg", "1265.jpg", "1271.jpg", "530.png", "1517.jpg", "732.jpg",
        "726.jpg", "727.jpg", "733.jpg", "1728.png", "1270.jpg", "1516.jpg", "1502.jpg", "928.jpg", "900.jpg",
        "1258.jpg", "914.jpg", "243.jpg", "1264.png", "525.jpg", "531.jpg", "257.jpg", "519.jpg", "1099.jpg",
        "1714.jpg", "280.jpg", "1072.jpg", "1066.jpg", "294.jpg", "1700.jpg", "337.jpg", "451.jpg", "445.jpg",
        "323.jpg", "479.jpg", "1660.jpg", "1106.jpg", "492.jpg", "486.jpg", "1112.jpg", "1674.jpg", "109.png",
        "1648.jpg", "653.jpg", "135.jpg", "647.jpg", "1489.jpg", "848.jpg", "690.jpg", "1304.jpg", "1462.jpg",
        "1310.jpg", "684.jpg", "874.jpg", "1338.jpg", "34.jpg", "860.jpg", "20.jpg", "876.jpg", "1448.jpg", "1460.jpg",
        "1306.jpg", "686.jpg", "1312.jpg", "1474.jpg", "679.jpg", "137.jpg", "889.jpg", "651.jpg", "645.jpg", "123.jpg",
        "1138.jpg", "490.jpg", "1104.jpg", "1662.jpg", "1676.jpg", "1110.jpg", "484.jpg", "862.png", "309.jpg",
        "1689.jpg", "453.jpg", "335.jpg", "321.jpg", "692.png", "447.jpg", "1058.jpg", "731.png", "1070.jpg", "282.jpg",
        "1716.jpg", "1702.jpg", "296.jpg", "1064.jpg", "725.png", "269.jpg", "527.jpg", "241.jpg", "255.jpg",
        "1528.jpg", "902.jpg", "916.jpg", "1514.jpg", "1272.jpg", "1266.jpg", "1500.jpg", "719.jpg", "1299.jpg",
        "730.jpg", "1717.png", "1298.jpg", "718.jpg", "1267.jpg", "1515.jpg", "1273.jpg", "917.jpg", "1529.jpg",
        "903.jpg", "254.jpg", "532.jpg", "526.jpg", "240.jpg", "268.jpg", "297.jpg", "1703.jpg", "724.png", "1065.jpg",
        "1071.jpg", "283.jpg", "1059.jpg", "320.jpg", "446.jpg", "452.jpg", "334.jpg", "1688.jpg", "308.jpg",
        "1677.jpg", "485.jpg", "1111.jpg", "1105.jpg", "491.jpg", "1663.jpg", "1139.jpg", "644.jpg", "122.jpg",
        "136.jpg", "650.jpg", "888.jpg", "678.jpg", "1313.jpg", "687.jpg", "1475.jpg", "1461.jpg", "693.jpg",
        "1307.jpg", "863.jpg", "23.jpg", "1449.jpg", "877.jpg", "37.jpg", "1303.jpg", "697.jpg", "442.png", "1465.jpg",
        "1471.jpg", "683.jpg", "1317.jpg", "33.jpg", "873.jpg", "1459.jpg", "27.jpg", "867.jpg", "654.jpg", "132.jpg",
        "126.jpg", "898.jpg", "640.jpg", "668.jpg", "1667.jpg", "495.jpg", "1101.jpg", "1115.jpg", "481.jpg",
        "1673.jpg", "1129.jpg", "330.jpg", "456.jpg", "324.jpg", "1698.jpg", "318.jpg", "287.jpg", "1713.jpg",
        "1075.jpg", "1061.jpg", "1707.jpg", "293.jpg", "1049.jpg", "244.jpg", "522.jpg", "536.jpg", "250.jpg",
        "278.jpg", "1277.jpg", "1511.jpg", "1505.jpg", "1263.jpg", "907.jpg", "1539.jpg", "720.jpg", "734.jpg",
        "1288.jpg", "708.jpg", "1289.jpg", "735.jpg", "1538.jpg", "912.jpg", "906.jpg", "1504.jpg", "1262.jpg",
        "1276.jpg", "279.jpg", "537.jpg", "1510.png", "251.jpg", "245.jpg", "523.jpg", "1048.jpg", "1060.jpg",
        "292.jpg", "1706.jpg", "1712.jpg", "286.jpg", "1074.jpg", "319.jpg", "1699.jpg", "443.jpg", "325.jpg",
        "331.jpg", "457.jpg", "1128.jpg", "1114.jpg", "1666.jpg", "1100.jpg", "494.jpg", "669.jpg", "127.jpg",
        "899.jpg", "655.jpg", "1672.png", "133.jpg", "866.jpg", "26.jpg", "872.jpg", "32.jpg", "1458.jpg", "1470.jpg",
        "1316.jpg", "682.jpg", "696.jpg", "1302.jpg", "1464.jpg", "1314.jpg", "18.jpg", "858.jpg", "1472.jpg",
        "327.png", "1300.jpg", "694.jpg", "24.jpg", "864.jpg", "30.jpg", "870.jpg", "643.jpg", "125.jpg", "131.jpg",
        "657.jpg", "119.jpg", "1499.jpg", "1670.jpg", "1116.jpg", "482.jpg", "496.jpg", "1102.jpg", "1664.jpg",
        "1658.jpg", "1466.png", "441.jpg", "455.jpg", "333.jpg", "1328.png", "469.jpg", "1704.jpg", "290.jpg",
        "1062.jpg", "737.png", "1076.jpg", "284.jpg", "1710.jpg", "1738.jpg", "253.jpg", "535.jpg", "521.jpg",
        "247.jpg", "509.jpg", "1089.jpg", "1260.jpg", "1506.jpg", "1512.jpg", "938.jpg", "910.jpg", "904.jpg",
        "1248.jpg", "723.jpg", "1249.jpg", "911.jpg", "1513.jpg", "1275.jpg", "939.jpg", "1261.jpg", "246.png",
        "1507.jpg", "1088.jpg", "508.jpg", "520.jpg", "246.jpg", "252.jpg", "534.jpg", "1739.jpg", "1077.jpg",
        "1711.jpg", "285.jpg", "291.jpg", "1705.jpg", "722.png", "1063.jpg", "468.jpg", "865.png", "454.jpg", "332.jpg",
        "326.jpg", "440.jpg", "695.png", "1301.png", "1659.jpg", "1103.jpg", "497.jpg", "1665.jpg", "1671.jpg",
        "483.jpg", "1498.jpg", "118.jpg", "130.jpg", "656.jpg", "1117.png", "642.jpg", "124.jpg", "871.jpg", "31.jpg",
        "25.jpg", "1329.jpg", "1467.jpg", "859.jpg", "19.jpg", "1315.jpg", "681.jpg", "1473.jpg", "42.jpg", "1428.jpg",
        "369.png", "4.jpg", "56.jpg", "816.jpg", "1372.jpg", "180.jpg", "1414.jpg", "1400.jpg", "194.jpg", "1366.jpg",
        "81.jpg", "619.jpg", "95.jpg", "1399.jpg", "625.jpg", "143.jpg", "157.jpg", "631.jpg", "1158.jpg", "382.jpg",
        "1170.jpg", "1164.jpg", "1602.jpg", "802.png", "341.jpg", "427.jpg", "433.jpg", "355.jpg", "1038.jpg",
        "1762.jpg", "1004.jpg", "584.jpg", "1010.jpg", "1776.jpg", "209.jpg", "235.jpg", "553.jpg", "792.png",
        "547.jpg", "221.jpg", "976.jpg", "1548.jpg", "962.jpg", "1206.jpg", "1560.jpg", "786.jpg", "779.jpg", "751.jpg",
        "989.jpg", "745.jpg", "590.png", "744.jpg", "988.jpg", "750.jpg", "778.jpg", "1575.jpg", "787.jpg", "1213.jpg",
        "1207.jpg", "793.jpg", "1561.jpg", "1549.jpg", "963.jpg", "977.jpg", "546.jpg", "220.jpg", "234.jpg", "552.jpg",
        "208.jpg", "1011.jpg", "585.jpg", "1777.jpg", "1763.jpg", "591.jpg", "1005.jpg", "1039.jpg", "432.jpg",
        "354.jpg", "340.jpg", "426.jpg", "368.jpg", "817.png", "1165.jpg", "1603.jpg", "397.jpg", "383.jpg", "1617.jpg",
        "156.jpg", "1171.png", "630.jpg", "624.jpg", "142.jpg", "1398.jpg", "94.jpg", "618.jpg", "80.jpg", "195.jpg",
        "1401.jpg", "1367.jpg", "1373.jpg", "1415.jpg", "181.jpg", "5.jpg", "803.jpg", "43.jpg", "1429.jpg", "55.jpg",
        "1359.jpg", "815.jpg", "41.jpg", "1365.jpg", "829.jpg", "1403.jpg", "197.jpg", "183.jpg", "1417.jpg",
        "1371.jpg", "96.jpg", "82.jpg", "632.jpg", "154.jpg", "140.jpg", "1167.png", "626.jpg", "168.png", "1629.jpg",
        "395.jpg", "1601.jpg", "1173.jpg", "1615.jpg", "381.jpg", "1198.jpg", "801.png", "418.jpg", "7.png", "356.jpg",
        "430.jpg", "424.jpg", "69.png", "342.jpg", "1749.jpg", "1775.jpg", "587.jpg", "1013.jpg", "1007.jpg", "593.jpg",
        "1761.jpg", "222.jpg", "544.jpg", "550.jpg", "1211.png", "236.jpg", "961.jpg", "975.jpg", "1239.jpg", "785.jpg",
        "1577.jpg", "1563.jpg", "949.jpg", "791.jpg", "1588.jpg", "746.jpg", "752.jpg", "753.jpg", "747.jpg",
        "1562.jpg", "1204.jpg", "790.jpg", "948.jpg", "784.jpg", "1210.jpg", "1576.jpg", "1238.jpg", "960.jpg",
        "551.jpg", "237.jpg", "545.jpg", "974.png", "579.jpg", "592.jpg", "1006.jpg", "1760.jpg", "1774.jpg",
        "1012.jpg", "586.jpg", "1589.png", "1748.jpg", "425.jpg", "343.jpg", "357.jpg", "431.jpg", "419.jpg",
        "1199.jpg", "1172.jpg", "380.jpg", "1614.jpg", "1600.jpg", "394.jpg", "1166.jpg", "1628.jpg", "141.jpg",
        "627.jpg", "633.jpg", "155.jpg", "83.jpg", "97.jpg", "169.jpg", "1416.jpg", "182.jpg", "1370.jpg", "828.jpg",
        "1364.jpg", "68.jpg", "196.jpg", "1402.jpg", "800.jpg", "40.jpg", "814.jpg", "1358.jpg", "54.jpg", "6.jpg",
        "1406.jpg", "192.jpg", "1360.jpg", "1374.jpg", "78.jpg", "838.jpg", "186.jpg", "1412.jpg", "2.jpg", "50.jpg",
        "810.jpg", "1348.jpg", "44.jpg", "804.jpg", "151.jpg", "637.jpg", "623.jpg", "145.jpg", "93.jpg", "87.jpg",
        "179.jpg", "1162.jpg", "390.jpg", "1604.jpg", "1610.jpg", "384.jpg", "1176.jpg", "1638.jpg", "435.jpg",
        "353.jpg", "347.jpg", "421.jpg", "409.jpg", "1189.jpg", "582.jpg", "1770.jpg", "1002.jpg", "1758.jpg",
        "1200.png", "541.jpg", "227.jpg", "233.jpg", "555.jpg", "1572.jpg", "958.jpg", "780.jpg", "794.jpg", "1566.jpg",
        "964.jpg", "1228.jpg", "569.png", "970.jpg", "743.jpg", "1016.png", "757.jpg", "1599.jpg", "1598.jpg",
        "756.jpg", "742.jpg", "971.jpg", "1229.jpg", "965.jpg", "1201.jpg", "795.jpg", "1567.jpg", "1573.jpg",
        "1215.jpg", "959.jpg", "568.jpg", "232.jpg", "781.png", "554.jpg", "540.jpg", "226.jpg", "1759.jpg", "1765.jpg",
        "597.jpg", "1003.jpg", "1017.jpg", "583.jpg", "1771.jpg", "1188.jpg", "408.jpg", "346.jpg", "1407.png",
        "1361.png", "420.jpg", "434.jpg", "1413.png", "352.jpg", "86.png", "1639.jpg", "1611.jpg", "1177.jpg",
        "1163.jpg", "1605.jpg", "391.jpg", "178.jpg", "92.jpg", "622.jpg", "144.jpg", "150.jpg", "385.png", "636.jpg",
        "805.jpg", "45.jpg", "1349.jpg", "3.jpg", "811.jpg", "839.jpg", "79.jpg", "1375.jpg", "187.jpg", "193.jpg",
        "185.jpg", "1411.jpg", "1377.jpg", "1363.jpg", "191.jpg", "47.jpg", "807.jpg", "53.jpg", "1.jpg", "1439.jpg",
        "146.jpg", "1161.png", "620.jpg", "634.jpg", "152.jpg", "1388.jpg", "608.jpg", "90.jpg", "1175.jpg", "1613.jpg",
        "387.jpg", "393.jpg", "1607.jpg", "1149.jpg", "84.png", "422.jpg", "1405.png", "344.jpg", "350.jpg", "436.jpg",
        "813.png", "378.jpg", "595.jpg", "1767.jpg", "1773.jpg", "581.jpg", "1015.jpg", "754.png", "1029.jpg",
        "556.jpg", "224.jpg", "542.jpg", "218.jpg", "1565.jpg", "797.jpg", "1203.jpg", "1217.jpg", "783.jpg",
        "1571.jpg", "230.png", "1559.jpg", "973.jpg", "967.jpg", "740.jpg", "998.jpg", "768.jpg", "999.jpg", "741.jpg",
        "755.jpg", "966.jpg", "219.png", "1558.jpg", "972.jpg", "231.png", "1570.jpg", "1564.jpg", "1202.jpg",
        "796.jpg", "225.jpg", "543.jpg", "782.png", "557.jpg", "1028.jpg", "769.png", "1772.jpg", "1014.jpg", "580.jpg",
        "594.jpg", "1000.jpg", "1766.jpg", "1438.png", "379.jpg", "351.jpg", "437.jpg", "423.jpg", "345.jpg",
        "1148.jpg", "1606.jpg", "392.jpg", "1160.jpg", "1174.jpg", "386.jpg", "1612.jpg", "91.jpg", "609.jpg", "85.jpg",
        "1389.jpg", "635.jpg", "153.jpg", "147.jpg", "621.jpg", "812.jpg", "52.jpg", "0.jpg", "806.jpg", "46.jpg",
        "1362.jpg", "190.jpg", "1404.jpg", "1410.jpg", "184.jpg", "1376.jpg"
};
#endif
static const int BASE_PERSONS_NUM = sizeof(BASE_PERSONS) / sizeof(char *);

static int addPersonsAndFeatures();

static int addFeature(char *personName, char *featureId, const char *filePath);

static int recognizeBmp();

static const char *GROUP_NAME = "SMALL_2000";
static Group sGroup;

static string sPicturesRoot = "../dumps_fd_p35595/20200102/";
static list<string> sPictures;

int main() {
    getPictureList(sPicturesRoot, sPictures);
    LOG(TAG, "sPictures size : %d", sPictures.size());
    if (sPictures.size() == 0) {
        return -1;
    }

    //step 1: set config path
    int status = setPersistencePath("../");
    if (status != OK) {
        LOG(TAG, "setPersistencePath error(%d) key(%s)", status);
        return 0;
    } else {
        LOG(TAG, "setPersistencePath ok");
    }

    //step 3: create FaceRegister Instance
    sFaceRegister = FaceRegister::createInstance();
    LOG(TAG, "sFaceRegister(%p)", sFaceRegister);
    if (!sFaceRegister) {
        LOG(TAG, "FaceRegister::createInstance error");
        return 0;
    }

    sFaceRecognize = FaceRecognize::createInstance(GROUP_NAME, TERMINAL);
    LOG(TAG, "sFaceRecognize(%p)", sFaceRecognize);
    if (!sFaceRecognize) {
        LOG(TAG, "FaceRecognize::createInstance error");
        return 0;
    }

    sFaceDetect = FaceDetect::createInstance(TERMINAL);
    LOG(TAG, "sFaceDetect(%p)", sFaceDetect);
    if (!sFaceDetect) {
        LOG(TAG, "FaceDetect::createInstance error");
        return 0;
    }

    //step 4: create group
    sGroup.name = GROUP_NAME;
    sGroup.modelType = MODEL_3K;
    //sGroup.modelType = MODEL_100K;
    status = sFaceRegister->createGroup(sGroup);
    if (status != OK && status != ERROR_EXISTED && status != ERROR_CLOUD_EXISTED_ERROR) {
        LOG(TAG, "createGroup error(%d)", status);
        return 0;
    }
    LOG(TAG, "createGroup OK, id(%s) name(%s) modelType(%d)", sGroup.id.c_str(), sGroup.name.c_str(),
        sGroup.modelType);

#if 0
    //step 5: addPersonsAndFeatures
    status = addPersonsAndFeatures();
    if (status != OK) {
        LOG(TAG, "addPersonsAndFeatures error(%d)", status);
        return 0;
    }
    LOG(TAG, "addPersonsAndFeatures OK");
#endif

    //step 9
    status = recognizeBmp();
    if (status != OK) {
        LOG(TAG, "recognizeBmp error(%d)", status);
        return 0;
    }
    LOG(TAG, "recognizeBmp OK");

    return 0;
}


int addPersonsAndFeatures() {
    printf("----0\n");
    char personName[PERSON_NAME_MAX_SIZE] = {0};
    char *featureName = "face1";

    for (int i = 0; i < BASE_PERSONS_NUM; i++) {
        memset(personName, 0, sizeof(personName));
        //memset(featureName, 0, sizeof(featureName));printf("----0a\n");
        sscanf(BASE_PERSONS[i], "%[^.]", personName);
        //sscanf(BASE_PERSONS[i], "%*[^_]_%[^.]", featureName);
        Person person;
        person.name = personName;
        int status = sFaceRegister->addPerson(sGroup.id, person);
        if (status != OK && status != ERROR_EXISTED && status != ERROR_CLOUD_EXISTED_ERROR) {
            LOG(TAG, "addPerson[%d] %s, error(%d)", i, personName, status);
            return status;
        }

        status = addFeature((char *) person.id.c_str(), featureName, (PICTURE_ROOT + BASE_PERSONS[i]).c_str());
        if (status != OK && status != ERROR_EXISTED && status != ERROR_CLOUD_EXISTED_ERROR) {
            LOG(TAG, "addFeature[%d] %s, status(%d)", i, featureName, status);
            return status;
        }
    }
    return OK;
}

int addFeature(char *personId, char *featureName, const char *filePath) {
    unsigned char *data = 0;
    int dataLen = 0;
    loadFile(data, dataLen, (char *) filePath);

    if (!data) {
        LOG(TAG, "loadFile filePath(%s) error", filePath);
        return FAILED;
    }

    Image image;
    image.data = data;
    image.format = COMPRESSED;
    image.dataLen = dataLen;

    list<Face> faces;
    int status = sFaceDetect->detectPicture(image, faces);

    if (status != OK || faces.size() == 0) {
        LOG(TAG, "detectPicture error(%d)", status);
        return status;
    }

    string feature;
    status = sFaceRegister->extractFeature(image, *faces.begin(), sGroup.modelType, feature);
    if (status != OK) {
        LOG(TAG, "extractFeature error(%d)", status);
        return status;
    }

    Feature feature1;
    feature1.name = featureName;
    feature1.feature = feature;
    status = sFaceRegister->addFeature(personId, feature1);
    if (status != OK && status != ERROR_EXISTED && status != ERROR_CLOUD_EXISTED_ERROR) {
        LOG(TAG, "addFeature error(%d)", status);
        return status;
    }

    return OK;
}

static map<int, RecognizeResult> sResults;

class RecognizeVideoListenerImp :
        public FaceRecognize::RecognizeVideoListener {
public:
    RecognizeVideoListenerImp() {

    }

    virtual ~RecognizeVideoListenerImp() {

    }

    virtual void onRecognized(Image &image, list<RecognizeResult> results) {
        LOG(TAG, "");
        int i = 0;
        for (list<RecognizeResult>::iterator it = results.begin(); it != results.end(); ++it) {
            LOG(TAG, "onRecognized[%d] BaseName(%s)  similarity(%f) track(%d)", i,
                it->personName.c_str(), it->similarity, it->trackId);
            sResults[it->trackId] = *it;
            i++;
        }
    }

    virtual void onVerified(Image &image, list<VerifyResult> results) {
        LOG(TAG, "");
    }
};

static int recognizeBmp() {
    LOG(TAG, "recognizeBmp");
    FaceRecognize::RecognizeVideoListener *listener = new RecognizeVideoListenerImp();
    sFaceRecognize->setRecognizeVideoListener(listener);

    //draw roi
    ali_face_engine::Rect roi;
    //FaceEngineConfig::FD_ROI(roi);

    int frameIndex = 0;
    int dropFrameCount = 0;
    for (auto &picturePath : sPictures) {

        string path = (sPicturesRoot + "/" + picturePath);
        unsigned char *data = 0;
        int dataLen = 0;
        loadFile(data, dataLen, (char *) path.c_str());
        if (!data) {
            LOG(TAG, "loadFile filePath(%s) error", path.c_str());
            return FAILED;
        }

        unsigned char *rgb = 0;
        int width = 0;
        int height = 0;
        Codec::bmpToRGB888(data, dataLen, rgb, width, height);
        free(data);
        data = 0;

        Image image;
        image.data = rgb;
        image.width = width;
        image.height = height;
        image.format = RGB888;

        list<Face> faceList;
        int error = sFaceDetect->detectVideo(image, faceList);
        LOG(TAG, "faceList:%d error=%d", faceList.size(), error);
        //sResults.clear();
        if (faceList.size()) {
            //error = sFaceRecognize->recognizePicture(image, faceList, sResults);
            error = sFaceRecognize->recognizeVideo(image, faceList);
        }
    }
    return 0;
}