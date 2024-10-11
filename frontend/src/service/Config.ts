import {signal, Signal} from "@preact/signals";
import {ConnStatus as btConnStatus, Service as btSvc} from "./Bluetooth";
import {number} from "prop-types";

const chcUUID = 0xff02;

interface fwVer {
    Major: number;
    Minor: number;
    Patch: number;
    Alpha: number;
}

export enum DisplayType {
    NONE = 0,
    MAX7219_32X16 = 1,
}

enum displayShowMode {
    SingleLine = 0,
    MultiLine = 1,
}

enum cfgID {
    appVerMajor = 0,
    appVerMinor = 1,
    appVerPatch = 2,
    appVerAlpha = 3,

    displayType = 30,

    rtcPinSCL = 70,
    rtcPinSDA = 71,

    displayShowMode = 128,
    displayMinBrightness = 129,
    displayMaxBrightness = 130,
    showTimeDuration = 131,
    showDateDuration = 132,
    showDOWDuration = 133,
    showAmbTempDuration = 134,
    showOdrTempDuration = 135,
    allowUnstableFirmware = 136,
}

export class Service {
    private btSvc: btSvc;

    private fwVer: Signal<fwVer> = signal({Major: 0, Minor: 0, Patch: 0, Alpha: 0});

    private displayType: Signal<DisplayType> = signal(DisplayType.NONE);

    private rtcPinSCL: Signal<number> = signal(0);
    private rtcPinSDA: Signal<number> = signal(0);

    private multilineMode: Signal<boolean> = signal(false);
    private displayMinBrightness: Signal<number> = signal(0);
    private displayMaxBrightness: Signal<number> = signal(15);

    private showTimeDuration: Signal<number> = signal(30);
    private showDateDuration: Signal<number> = signal(5);
    private showDOWDuration: Signal<number> = signal(5);
    private showAmbTempDuration: Signal<number> = signal(5);
    private showOdrTempDuration: Signal<number> = signal(5);
    private allowUnstableFirmware: Signal<boolean> = signal(false);

    constructor(btSvc: btSvc) {
        this.btSvc = btSvc;
        setInterval(this.fetch.bind(this), 1000);
    }

    get FirmwareVersionString(): string {
        let s = `${this.fwVer.value.Major}.${this.fwVer.value.Minor}.${this.fwVer.value.Patch}`;

        if (this.fwVer.value.Alpha != 0) {
            s += `-alpha${this.fwVer.value.Alpha}`;
        }

        return s;
    }

    get DisplayType(): DisplayType {
        return this.displayType.value;
    }

    get RTPPinSCL(): number {
        return this.rtcPinSCL.value;
    }

    get RTPPinSDA(): number {
        return this.rtcPinSDA.value;
    }

    get DisplayMinBrightness(): number {
        return this.displayMinBrightness.value;
    }

    get DisplayMaxBrightness(): number {
        return this.displayMaxBrightness.value;
    }

    get MultilineMode(): boolean {
        return this.multilineMode.value
    }

    get ShowTimeDuration(): number {
        return this.showTimeDuration.value;
    }

    get ShowDateDuration(): number {
        return this.showDateDuration.value;
    }

    get ShowOdrTempDuration(): number {
        return this.showOdrTempDuration.value;
    }

    get AllowUnstableFirmware(): boolean {
        return this.allowUnstableFirmware.value;
    }

    async SetRTCPinSCL(v: number) {
        this.rtcPinSCL.value = v;
        return this.setCfgVal(cfgID.rtcPinSCL, v)
    }

    async SetRTCPinSDA(v: number) {
        this.rtcPinSDA.value = v;
        return this.setCfgVal(cfgID.rtcPinSDA, v)
    }

    async SetDisplayMinBrightness(v: number) {
        this.displayMinBrightness.value = v;
        return this.setCfgVal(cfgID.displayMinBrightness, v)
    }

    async SetDisplayMaxBrightness(v: number) {
        this.displayMaxBrightness.value = v;
        return this.setCfgVal(cfgID.displayMaxBrightness, v)
    }

    async SetMultilineMode(v: boolean): Promise<void> {
        this.multilineMode.value = v;
        if (v) {
            return this.setCfgVal(cfgID.displayShowMode, displayShowMode.MultiLine)
        } else {
            return this.setCfgVal(cfgID.displayShowMode, displayShowMode.SingleLine)
        }
    }

    async SetShowTimeDuration(v: number): Promise<void> {
        this.showTimeDuration.value = v;
        return this.setCfgVal(cfgID.showTimeDuration, v)
    }

    async SetShowDateDuration(v: number): Promise<void> {
        this.showDateDuration.value = v;
        return this.setCfgVal(cfgID.showDateDuration, v)
    }

    async SetShowOdrTempDuration(v: number): Promise<void> {
        this.showOdrTempDuration.value = v;
        return this.setCfgVal(cfgID.showOdrTempDuration, v)
    }

    async SetAllowUnstableFirmware(v: boolean): Promise<void> {
        this.allowUnstableFirmware.value = v;
        return this.setCfgVal(cfgID.allowUnstableFirmware, Number(v))
    }

    private async setCfgVal(id: number, val: number) {
        const d = new Uint8Array(2);

        d[0] = id;
        d[1] = val;

        return this.btSvc.write(chcUUID, d);
    }

    private async fetch() {
        if (this.btSvc.connStatus != btConnStatus.CONNECTED) {
            return;
        }

        let data = await this.btSvc.read(chcUUID);

        this.fwVer.value = {
            Major: data.getInt8(cfgID.appVerMajor),
            Minor: data.getInt8(cfgID.appVerMinor),
            Patch: data.getInt8(cfgID.appVerPatch),
            Alpha: data.getInt8(cfgID.appVerAlpha),
        };

        this.displayType.value = data.getInt8(cfgID.displayType);

        this.rtcPinSCL.value = data.getInt8(cfgID.rtcPinSCL);
        this.rtcPinSDA.value = data.getInt8(cfgID.rtcPinSDA);

        this.multilineMode.value = data.getInt8(cfgID.displayShowMode) == displayShowMode.MultiLine;

        this.showTimeDuration.value = data.getInt8(cfgID.showTimeDuration);
        this.showDateDuration.value = data.getInt8(cfgID.showDateDuration);
        this.showDOWDuration.value = data.getInt8(cfgID.showDOWDuration);
        this.showAmbTempDuration.value = data.getInt8(cfgID.showAmbTempDuration);
        this.showOdrTempDuration.value = data.getInt8(cfgID.showOdrTempDuration);
        this.allowUnstableFirmware.value = Boolean(data.getInt8(cfgID.allowUnstableFirmware));
    }
}
