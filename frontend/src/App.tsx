import * as React from 'react';
import Router from 'preact-router';

import * as BTSvc from './service/Bluetooth';
import * as WiFiSvc from './service/WiFi';
import * as ConfigSvc from './service/Config';

import Home from './routes/Home';
import WiFi from './routes/WiFi';
import Display from './routes/Display';
import Developer from './routes/Developer';

export default class App extends React.Component <{}, {}> {
    private readonly btSvc: BTSvc.Service;
    private readonly wifiSvc: WiFiSvc.Service;
    private readonly cfgSvc: ConfigSvc.Service;

    constructor() {
        super();

        this.btSvc = new BTSvc.Service(0xffff);
        this.wifiSvc = new WiFiSvc.Service(this.btSvc);
        this.cfgSvc = new ConfigSvc.Service(this.btSvc);
    }

    render(): JSX.Element {
        return (
            <Router>
                <Home path={"/device"} btSvc={this.btSvc} cfgSvc={this.cfgSvc}/>
                <WiFi path={"/device/wifi"} btConnStatus={this.btSvc.connStatus} wifiSvc={this.wifiSvc}/>
                <Display path={"/device/display"} btConnStatus={this.btSvc.connStatus} cfgSvc={this.cfgSvc}/>
                <Developer path={"/device/developer"} btConnStatus={this.btSvc.connStatus} cfgSvc={this.cfgSvc}/>
            </Router>
        );
    }
}
