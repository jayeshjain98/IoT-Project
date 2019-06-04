# -*- coding: utf-8 -*-
"""
Created on Thu Jun 28 11:43:23 2018

@author: rohit
"""

from flask import Flask, request, json, Response,jsonify
from pymongo import MongoClient
import urllib2




app = Flask(__name__)

def sms(number,temp): 
    print "sending sms"
    # Alert Message to be sent
    if temp == 1:
        print "temp 1"
        message = "Your Room's Power Supply is ON"
        message = "+".join(message.split(' '))
    else :
        print "temp 0"
        message = "Your Room's Power Supply is OFF"
        message = message.replace(" ", "+")
    # Adding number and message to SMS url.(P.S. rest all the attributes remains same as per service provider)
    url ='http://sms.dataoxytech.com/index.php/smsapi/httpapi/?uname=sylvester007&password=forskmnit&sender=FORSKT&receiver={0}&route=TA&msgtype=1&sms={1}'.format(number,message)

    try:
        send_sms = urllib2.urlopen(url)
        print send_sms# Hitting the SMS url
    except Exception as e:
        print "exception", str(e)
        return "Error:",str(e)
    return send_sms.read() # Returns SMS Response


@app.route('/requestType', methods=['GET', 'POST', 'PUT', 'DELETE'])
def defautMethods():
    if request.method == 'GET' :
        print "ECHO: GET\n", request.headers
        client = MongoClient('localhost', 27017)
        my_db = client.my_data
        collection = my_db.Data1
        result = list(collection.find().sort([("_id", -1)]))
        data_list = {}
        for i in result[0:1] :
            data_list["power"] = i["PowerStatus"]
            data_list["humidity"] = i["Humidity"]
            data_list["intensity"] = i["Intensity"]
            data_list["temprature"] = i["Temprature"]
        
        #{"powerStatus" : str(data_list["power"])}

        
        return str(data_list["power"])
    
    
    elif request.method == 'POST' and request.headers['Content-Type'] == 'application/json':
        print "in html form data post", request.headers
        powerStatus = request.json["powerStatus"]
        lightStatus = request.json["intensity"]
        temprature = request.json["temprature"]
        humidity = request.json["humidity"]
        
        
        
        
        
        data = {
        'PowerStatus'  : powerStatus,
        'Intensity' : lightStatus,
        'Temprature' : temprature,
        'Humidity' : humidity
        }
        #js = json.dumps(data)
        client = MongoClient('localhost', 27017)
        my_db = client.my_data

        my_db.Data1.insert(data)
        
        #resp = Response(js, status=200, mimetype='application/json')
        return jsonify({"status" : 200, "Message" : "Your Records Has been updated Successfully"})
        
         
    elif request.method == 'PUT' and request.headers['Content-Type'] == 'application/json':
        print "put method", request.headers
        name = request.json["name"]
        email = request.json["email"]
        
        
            
        data = {
        'name'  : name,
        'email' : email
        }
        js = json.dumps(data)
        
        resp = Response(js, status=200, mimetype='application/json')
        return resp
        #return "ECHO: PUT\n"
    
    elif request.method == 'DELETE':
        
        name = request.json["name"]
        print "got name", name
        client = MongoClient('localhost', 27017)
        my_db = client.Thinkspeak
        collection = my_db.Data1
        result = list(collection.find().sort([("_id", -1)]))
        result[0]
        
        print "deleting collection", collection.delete_many({"username" : name})
                
        
        
        #call your methods to delete the existing entry from datbase
        return "deleted name : " + name
        
   
    else:
        return "unexpected method called"
@app.route('/getSensorData', methods=['GET'])
def getData():
    if request.method == 'GET' :
        print "ECHO: GET\n", request.headers
        client = MongoClient('localhost', 27017)
        my_db = client.my_data
        collection = my_db.Data1
        result = list(collection.find().sort([("_id", -1)]))
        data_list = []
        data_dict = {}
        for i in result:
            print i
            data_dict["power"] = i["PowerStatus"]
            data_dict["humidity"] = i["Humidity"]
            data_dict["intensity"] = i["Intensity"]
            data_dict["temprature"] = i["Temprature"]
            data_list.append(data_dict)
            
        
        #{"powerStatus" : str(data_list["power"])}
        
        return jsonify(data_list)
    return "unexpected Input"

if __name__ == '__main__':
    app.run(debug=True,host='0.0.0.0',port=80)


