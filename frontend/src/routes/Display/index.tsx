import * as React from "react";
import * as BTSvc from "../../service/Bluetooth";
import * as ConfigSvc from "../../service/Config";
import Grid from "@mui/material/Grid";
import CssBaseline from "@mui/material/CssBaseline";
import {Link, route} from "preact-router";

import Stack from "@mui/material/Stack";
import Box from "@mui/material/Box";
import Button from "@mui/material/Button";
import FormControlLabel from "@mui/material/FormControlLabel";
import Checkbox from "@mui/material/Checkbox";
import Select from "@mui/material/Select";
import MenuItem from '@mui/material/MenuItem';
import FormControl from "@mui/material/FormControl";
import InputLabel from "@mui/material/InputLabel";

import ArrowBackIcon from "@mui/icons-material/ArrowBack";

interface Props {
    btConnStatus: BTSvc.ConnStatus
    cfgSvc: ConfigSvc.Service
}

interface State {
}

const durationValues = [
    {value: 0, label: "Don't show"},
    {value: 5, label: '5 sec'},
    {value: 15, label: '15 sec'},
    {value: 30, label: '30 sec'},
    {value: 45, label: '45 sec'},
    {value: 60, label: '1 min'},
];

const brightnessValues = [
    {value: 0, label: "1"},
    {value: 1, label: "2"},
    {value: 2, label: "3"},
    {value: 3, label: "4"},
    {value: 4, label: "5"},
    {value: 5, label: "6"},
    {value: 6, label: "7"},
    {value: 7, label: "8"},
    {value: 8, label: "9"},
    {value: 9, label: "10"},
    {value: 10, label: "11"},
    {value: 11, label: "12"},
    {value: 12, label: "13"},
    {value: 13, label: "14"},
    {value: 14, label: "15"},
    {value: 15, label: "16"},
]

export default class Display extends React.Component<Props, State> {
    render(): React.JSX.Element {
        return <Grid container
                     spacing={0}
                     direction="column"
                     alignItems="center"
                     justifyContent="center"
                     sx={{minHeight: '100vh'}}
        >
            <CssBaseline/>

            {this.props.btConnStatus != BTSvc.ConnStatus.CONNECTED
                ?
                <Box>
                    You're not connected to your Cronus device.
                    Click <Link href={"/device/"}>here</Link> to connect.
                </Box>
                :
                <Stack direction={"column"} spacing={3}>
                    <FormControlLabel
                        label="Multiline mode"
                        control={
                            <Checkbox checked={this.props.cfgSvc.MultilineMode}
                                      onChange={(_, v) => this.props.cfgSvc.SetMultilineMode(v)}
                            />
                        }
                    />

                    <FormControl variant="standard">
                        <InputLabel id="min-brightness-label">Minimum brightness</InputLabel>
                        <Select
                            labelId="min-brightness-duration"
                            id="min-brightness-duration-select"
                            value={this.props.cfgSvc.DisplayMinBrightness}
                            label="minimum brightness level"
                            onChange={(_, v) => this.props.cfgSvc.SetDisplayMinBrightness(v.props.value)}
                        >
                            {brightnessValues.map((opt) => {
                                    if (opt.value <= this.props.cfgSvc.DisplayMaxBrightness) {
                                        return <MenuItem key={opt.value} value={opt.value}>{opt.label}</MenuItem>
                                    }
                                }
                            )}
                        </Select>
                    </FormControl>

                    <FormControl variant="standard">
                        <InputLabel id="max-brightness-label">Maximum brightness</InputLabel>
                        <Select
                            labelId="max-brightness-duration"
                            id="max-brightness-duration-select"
                            value={this.props.cfgSvc.DisplayMaxBrightness}
                            label="Maximum brightness level"
                            onChange={(_, v) => this.props.cfgSvc.SetDisplayMaxBrightness(v.props.value)}
                        >
                            {brightnessValues.map(opt => (
                                <MenuItem key={opt.value} value={opt.value}>{opt.label}</MenuItem>
                            ))}
                        </Select>
                    </FormControl>

                    {!this.props.cfgSvc.MultilineMode && (
                        <FormControl variant="standard">
                            <InputLabel id="show-time-duration-label">Show time</InputLabel>
                            <Select
                                labelId="show-time-duration"
                                id="show-time-duration-select"
                                value={this.props.cfgSvc.ShowTimeDuration}
                                label="Show time"
                                onChange={(_, v) => this.props.cfgSvc.SetShowTimeDuration(v.props.value)}
                            >
                                {durationValues.map(opt => (
                                    <MenuItem key={opt.value} value={opt.value}>{opt.label}</MenuItem>
                                ))}
                            </Select>
                        </FormControl>
                    )}

                    <FormControl variant="standard">
                        <InputLabel id="show-date-duration-label">Show date</InputLabel>
                        <Select
                            labelId="show-time-duration"
                            id="show-time-duration-select"
                            value={this.props.cfgSvc.ShowDateDuration}
                            label="Show time"
                            onChange={(_, v) => this.props.cfgSvc.SetShowDateDuration(v.props.value)}
                        >
                            {durationValues.map(opt => (
                                <MenuItem key={opt.value} value={opt.value}>{opt.label}</MenuItem>
                            ))}
                        </Select>
                    </FormControl>

                    <FormControl variant="standard">
                        <InputLabel id="show-odr-temp-duration-label">Show outdoor temp</InputLabel>
                        <Select
                            labelId="show-odr-temp-duration"
                            id="show-odr-duration-select"
                            value={this.props.cfgSvc.ShowOdrTempDuration}
                            label="Show outdoor temperature"
                            onChange={(_, v) => this.props.cfgSvc.SetShowOdrTempDuration(v.props.value)}
                        >
                            {durationValues.map(opt => (
                                <MenuItem key={opt.value} value={opt.value}>{opt.label}</MenuItem>
                            ))}
                        </Select>
                    </FormControl>
                    <Button variant={"outlined"} startIcon={<ArrowBackIcon/>} onClick={() => route("/device")}>
                        Back
                    </Button>
                </Stack>
            }
        </Grid>;
    }
}