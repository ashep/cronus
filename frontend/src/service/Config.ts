import {signal, Signal} from "@preact/signals";
import {ConnStatus as btConnStatus, Service as btSvc} from "./Bluetooth";
import {bool} from "prop-types";

const chcUUID = 0xff02;

interface fwVer {
    Major: number;
    Minor: number;
    Patch: number;
}

export enum DisplayType {
    NONE = 0,
    MAX7219_32X8 = 1,
    MAX7219_32X16 = 2,
}

enum cfgID {
    versionMajor = 0,
    versionMinor = 1,
    versionPatch = 2,
    flags1 = 3,

    displayType = 10,
    displayMinBrightness = 11,
    displayMaxBrightness = 12,

    showTimeDuration = 20,
    showDateDuration = 21,
    showDOWDuration = 22,
    showAmbTempDuration = 23,
    showOdrTempDuration = 24,
}

enum cfgBitsSettings1 {
    multilineMode = 0,
    showDate = 1,
    showAmbientTemp = 2,
    showWeatherTemp = 3,
}

export class Service {
    private btSvc: btSvc;
    private fwVer: Signal<fwVer> = signal({Major: 0, Minor: 0, Patch: 0});
    private displayType: Signal<DisplayType> = signal(DisplayType.NONE);
    private displayMinBrightness: Signal<number> = signal(0);
    private displayMaxBrightness: Signal<number> = signal(15);

    private multilineMode: Signal<boolean> = signal(false);

    private showTimeDuration: Signal<number> = signal(30);
    private showDateDuration: Signal<number> = signal(5);
    private showDOWDuration: Signal<number> = signal(5);
    private showAmbTempDuration: Signal<number> = signal(5);
    private showOdrTempDuration: Signal<number> = signal(5);

    private data: DataView = new DataView(new ArrayBuffer(64)); // must be the same as firmware's CRONUS_CFG_CFG_BUF_LEN

    constructor(btSvc: btSvc) {
        this.btSvc = btSvc;
        setInterval(this.fetch.bind(this), 1000);
    }

    get FirmwareVersionString(): string {
        return `${this.fwVer.value.Major}.${this.fwVer.value.Minor}.${this.fwVer.value.Patch}`;
    }

    get DisplayType(): DisplayType {
        return this.displayType.value;
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

    async SetDisplayMinBrightness(v: number) {
        this.displayMinBrightness.value = v;
        this.data.setInt8(cfgID.displayMinBrightness, v);
        return this.btSvc.write(chcUUID, this.data);
    }

    async SetDisplayMaxBrightness(v: number) {
        this.displayMaxBrightness.value = v;
        this.data.setInt8(cfgID.displayMaxBrightness, v);
        return this.btSvc.write(chcUUID, this.data);
    }

    async SetMultilineMode(v: boolean): Promise<void> {
        this.multilineMode.value = v;

        this.data.setInt8(
            cfgID.flags1,
            this.setBit(this.data.getInt8(cfgID.flags1), cfgBitsSettings1.multilineMode, v)
        );

        return this.btSvc.write(chcUUID, this.data);
    }

    async SetShowTimeDuration(v: number): Promise<void> {
        this.showTimeDuration.value = v;
        this.data.setInt8(cfgID.showTimeDuration, v);
        return this.btSvc.write(chcUUID, this.data);
    }

    async SetShowDateDuration(v: number): Promise<void> {
        this.showDateDuration.value = v;
        this.data.setInt8(cfgID.showDateDuration, v);
        return this.btSvc.write(chcUUID, this.data);
    }

    async SetShowDOWDuration(v: number): Promise<void> {
        this.showDOWDuration.value = v;
        this.data.setInt8(cfgID.showDOWDuration, v);
        return this.btSvc.write(chcUUID, this.data);
    }

    async SetShowAmbTempDuration(v: number): Promise<void> {
        this.showAmbTempDuration.value = v;
        this.data.setInt8(cfgID.showAmbTempDuration, v);
        return this.btSvc.write(chcUUID, this.data);
    }

    async SetShowOdrTempDuration(v: number): Promise<void> {
        this.showOdrTempDuration.value = v;
        this.data.setInt8(cfgID.showOdrTempDuration, v);
        return this.btSvc.write(chcUUID, this.data);
    }

    private async fetch() {
        if (this.btSvc.connStatus != btConnStatus.CONNECTED) {
            return;
        }

        this.data = await this.btSvc.read(chcUUID);

        this.fwVer.value = {
            Major: this.data.getInt8(cfgID.versionMajor),
            Minor: this.data.getInt8(cfgID.versionMinor),
            Patch: this.data.getInt8(cfgID.versionPatch),
        };

        this.displayType.value = this.data.getInt8(cfgID.displayType);
        this.multilineMode.value = Boolean(this.data.getInt8(cfgID.flags1) & 1 << cfgBitsSettings1.multilineMode);

        this.showTimeDuration.value = this.data.getInt8(cfgID.showTimeDuration);
        this.showDateDuration.value = this.data.getInt8(cfgID.showDateDuration);
        this.showDOWDuration.value = this.data.getInt8(cfgID.showDOWDuration);
        this.showAmbTempDuration.value = this.data.getInt8(cfgID.showAmbTempDuration);
        this.showOdrTempDuration.value = this.data.getInt8(cfgID.showOdrTempDuration);
    }


    private setBit(byte: number, pos: number, v: boolean): number {
        if (v) {
            byte |= 1 << pos;
        } else {
            byte &= ~(1 << pos);
        }

        return byte;
    }
}
