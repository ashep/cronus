import {signal, Signal} from "@preact/signals";

export enum ConnStatus {
    DISCONNECTED,
    CONNECTING,
    CONNECTED
}

export class Service {
    private readonly svcUUID: number;
    private device?: BluetoothDevice;
    private server?: BluetoothRemoteGATTServer;
    private service?: BluetoothRemoteGATTService;

    private cnStatus: Signal<ConnStatus>;

    private onConnect?: () => void;
    private onDisconnect?: () => void;

    get connectedDeviceName(): string | null {
        return this.server?.device?.name
    }

    get connStatus(): ConnStatus {
        return this.cnStatus.value;
    }

    constructor(svcUUID: number, onConnect?: () => void, onDisconnect?: () => void) {
        this.svcUUID = svcUUID;

        this.onConnect = onConnect;
        this.onDisconnect = onDisconnect;

        this.cnStatus = signal<ConnStatus>(ConnStatus.DISCONNECTED);

        setInterval(this.connStatusChecker.bind(this), 500);
    }

    private connStatusChecker(): void {
        if (!(this.server && this.server.connected) && this.cnStatus.value !== ConnStatus.CONNECTING) {
            this.setConnStatus(ConnStatus.DISCONNECTED);
        }
    }

    private setConnStatus(s: ConnStatus): void {
        if (s == this.cnStatus.value) {
            return;
        }

        this.cnStatus.value = s;

        switch (s) {
            case ConnStatus.CONNECTED:
                this.onConnect && this.onConnect();
                break
            case ConnStatus.DISCONNECTED:
                this.onDisconnect && this.onDisconnect();
                break
        }
    }

    async connect() {
        if (this.server?.connected) {
            throw ("already connected");
        }

        if (!(await navigator.bluetooth.getAvailability())) {
            throw ("bluetooth is not available");
        }

        this.setConnStatus(ConnStatus.CONNECTING);

        try {
            this.device = await navigator.bluetooth.requestDevice({
                filters: [
                    {services: [0xFFFF]},
                    {namePrefix: "Cronus"},
                ],
            });

            this.server = await this.device.gatt?.connect();
            this.service = await this.server?.getPrimaryService(this.svcUUID);

            this.setConnStatus(ConnStatus.CONNECTED);
        } catch (e) {
            this.setConnStatus(ConnStatus.DISCONNECTED);
            throw e;
        }
    }

    async read(chcUUID: number): Promise<DataView> {
        if (!this.server || !this.server.connected) {
            this.setConnStatus(ConnStatus.DISCONNECTED);
            throw "not connected";
        }

        return (await this.service?.getCharacteristic(chcUUID)).readValue();
    }

    async write(chcUUID: number, data: BufferSource): Promise<void> {
        if (!this.device || !this.server.connected) {
            this.setConnStatus(ConnStatus.DISCONNECTED);
            throw "not connected";
        }

        return (await this.service?.getCharacteristic(chcUUID)).writeValueWithoutResponse(data);
    }
}