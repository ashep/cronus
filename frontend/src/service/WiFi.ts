import {signal, Signal} from "@preact/signals";

import {Service as btSvc} from "./Bluetooth";
import {ConnStatus as btConnStatus} from "./Bluetooth";

const wifiBTChcUUID = 0xff01;

export enum Op {
    NOP,
    SCAN,
    CONNECT,
    DISCONNECT,
}

export enum ConnStatus {
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
}

export enum ErrorReason {
    NO_ERROR,
    UNKNOWN = 16,
}

export interface Credentials {
    ssid: string;
    password: string;
}

export class Service {
    private _btSvc: btSvc;
    private _connStatus: Signal<ConnStatus> = signal(ConnStatus.DISCONNECTED);
    private _ssidList: Signal<Array<string>> = signal([]);
    private _connectedSSID: Signal<string> = signal("");
    private _lastError: Signal<number> = signal(ErrorReason.NO_ERROR);
    private _nextScanTime: number = 0;

    get connStatus(): ConnStatus {
        return this._connStatus.value;
    }

    get ssidList(): Array<string> {
        return this._ssidList.value;
    }

    get connectedSSID(): string {
        return this._connectedSSID.value;
    }

    constructor(btSvc: btSvc) {
        this._btSvc = btSvc;
        setInterval(this.fetchWiFiStatus.bind(this), 3000);
    }

    private trimByteStr(s: string): string {
        let endPos = s.length - 1;
        for (let j = 0; j < s.length; j++) {
            if (s.codePointAt(j) == 0) {
                endPos = j;
                break;
            }
        }

        return s.substring(0, endPos);
    }

    private async fetchWiFiStatus(): Promise<void> {
        if (this._btSvc.connStatus != btConnStatus.CONNECTED) {
            return;
        }

        // byte 0:       state: disconnected(0), scanning(1), connecting(2), connected(3), error(4)
        // byte 1-32:    connected SSID or error description
        // byte 33-64:   scanned SSID 1
        // byte 65-96:   scanned SSID 2
        // byte 97-128:  scanned SSID 3
        // byte 129-160: scanned SSID 4
        // byte 161-192: scanned SSID 5
        const data = await this._btSvc.read(wifiBTChcUUID);

        const dec = new TextDecoder("utf-8");
        const newList = [];

        const d = data.getInt8(0);
        const st: ConnStatus = d & 0x0f; // low 4 bits
        const errReason = d >> 4; // high 4 bits

        let offset = 33;
        for (let i = 0; i < 5; i++) {
            const ssid = this.trimByteStr(dec.decode(data.buffer.slice(offset, offset + 32)));
            if (ssid != "") {
                newList.push(ssid);
            }
            offset += 32;
        }

        this._connStatus.value = st;
        this._ssidList.value = newList;
        this._lastError.value = errReason;

        switch (st) {
            case ConnStatus.DISCONNECTED:
                if (Date.now() >= this._nextScanTime) {
                    this.requestWiFiScan();
                    this._nextScanTime = Date.now() + 7000;
                }
                break;

            case ConnStatus.CONNECTED:
                this._connectedSSID.value = this.trimByteStr(dec.decode(data.buffer.slice(1, 33)));
                break;
        }
    }

    async requestWiFiScan(): Promise<void> {
        const val = new Int8Array(1);
        val.set([Op.SCAN], 0);

        return this._btSvc.write(wifiBTChcUUID, val);
    }

    async requestWiFiConnect(c: Credentials): Promise<void> {
        this._connStatus.value = ConnStatus.CONNECTING;

        const enc = new TextEncoder();

        const val = new Int8Array(97);
        val.set([Op.CONNECT], 0);
        val.set(enc.encode(c.ssid), 1)
        val.set(enc.encode(c.password), 33)

        return this._btSvc.write(wifiBTChcUUID, val);
    }

    async requestWiFiDisconnect(): Promise<void> {
        this._connectedSSID.value = "";

        const val = new Int8Array(1);
        val.set([Op.DISCONNECT], 0);

        return this._btSvc.write(wifiBTChcUUID, val);
    }
}
