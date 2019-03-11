import pymysql
import ha_util

connection = pymysql.connect(host='localhost',
                 user='leo',
                 password='golang*',
                 db='homeauto',
                 charset='utf8')
                 
row_count = ha_util.inactive_all_device(connection)
row_count = ha_util.active_device(connection, '1234', '12345678')
print(row_count)

connection.close()
