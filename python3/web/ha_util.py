import pymysql

def inactive_all_device(connection):
    row_count = 0

    with connection.cursor() as cursor:
        sql = "UPDATE device SET device_active=0"
        row_count = cursor.execute(sql, )
        connection.commit()

    return row_count

def active_device(connection, short_address, ieee_address):
    row_count = 0

    with connection.cursor() as cursor:
        sql = "SELECT device_id from device where device_ieee_address = %s"
        cursor.execute(sql, ieee_address)
        result = cursor.fetchone()
        if result == None:
            sql = "INSERT INTO device (device_short_address, device_ieee_address, device_active) VALUES (%s, %s, 1);"
            row_count = cursor.execute(sql, (short_address, ieee_address))
            connection.commit()
        else:
            sql = "UPDATE device SET device_short_address=%s, device_active=1 where device_ieee_address = %s"
            row_count = cursor.execute(sql, (short_address, ieee_address))
            connection.commit()

    return row_count
