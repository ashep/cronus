// Author:  Oleksandr Shepetko
// Email:   a@shepetko.com
// License: MIT

package service

import (
	"encoding/json"
	"log"
	"net/http"
	"time"
	_ "time/tzdata"
)

type RootHandlerResponse struct {
	Second int `json:"second"`
	Minute int `json:"minute"`
	Hour   int `json:"hour"`
	Dow    int `json:"dow"`
	Day    int `json:"day"`
	Month  int `json:"month"`
	Year   int `json:"year"`

	Weather   bool    `json:"weather"`
	Temp      float64 `json:"temp"`
	FeelsLike float64 `json:"feels_like"`
}

func (s *Service) RootHandler(w http.ResponseWriter, r *http.Request) {
	rAddr := ""
	rAddr = r.Header.Get("cf-connecting-ip")
	if rAddr == "" {
		rAddr = r.Header.Get("x-forwarded-for")
	}
	if rAddr == "" {
		rAddr = r.RemoteAddr
	}

	if rAddr == "" {
		log.Printf("failed to determine remote ip address")
		w.WriteHeader(http.StatusInternalServerError)
		return
	}

	// Get location
	geoData, err := s.geoIP.Get(rAddr)
	if err != nil {
		log.Printf("geoip error: %v", err)
		w.WriteHeader(http.StatusInternalServerError)
		return
	}

	log.Printf("req: addr=%s; ua=%s; city=%s", rAddr, r.Header.Get("User-Agent"), geoData.City)

	// Get localized time
	tz, err := time.LoadLocation(geoData.TimeZone)
	if err != nil {
		log.Printf("failed to determine time zone: %v", err)
	}
	t := time.Now().In(tz)

	// Fix weekday number
	dow := int(t.Weekday())
	if dow == 0 {
		dow = 7
	}

	// Prepare response
	resp := RootHandlerResponse{
		Second: t.Second(),
		Minute: t.Minute(),
		Hour:   t.Hour(),
		Dow:    dow,
		Day:    t.Day(),
		Month:  int(t.Month()),
		Year:   t.Year() - 2000,
	}

	// Add weather data
	weatherData, err := s.weather.Get(geoData.Latitude, geoData.Longitude)
	if err == nil {
		resp.Weather = true
		resp.Temp = weatherData.Temp
		resp.FeelsLike = weatherData.FeelsLike
	} else {
		log.Printf("failed to get weather: %v", err)
	}

	d, err := json.Marshal(resp)
	if err != nil {
		log.Printf("response marshal error: %v", err)
		w.WriteHeader(http.StatusInternalServerError)
		return
	}

	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(http.StatusOK)
	if _, err = w.Write(d); err != nil {
		log.Printf("response write error: %v", err)
	}

	log.Printf("rsp: addr=%s; data=%s", rAddr, d)
}
