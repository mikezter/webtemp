class Webtemp
  constructor: ->
    @values = {tmp: 0, hum: 0}
    @updateView()
    window.setInterval(@getValues, 1000);

  getValues: ->
    $.getJSON "http://arduino.local/data/get", (data) =>
      @values = data.value
      @updateView()

  updateView: ->
    $(".tmp").html(@values.tmp)
    $(".hum").html(@values.hum)

  $ -> new Webtemp()
