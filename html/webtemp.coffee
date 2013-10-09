class Webtemp
  basePath: "/data"

  constructor: ->
    @values = {tmp: '23.80', hum: '49.10'}
    @getValues()
    @updateView()

    $('.command').on 'click', @doApi

    window.setInterval (=> @getValues()), 2000

  getValues: ->
    $.getJSON "#{@basePath}/get", (data) =>
      @values = data.value
      @updateView()

  doApi: ->
    c = $(@).data 'c'
    $.getJSON "#{@basePath}/put/c/#{c}"

  updateView: ->

    $(".tmp").html @values.tmp.slice(0,-1)
    $(".hum").html @values.hum.slice(0,-1)

  $ -> new Webtemp()
