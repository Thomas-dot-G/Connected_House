class AppliController < ApplicationController
	before_filter :require_user
	skip_before_filter  :verify_authenticity_token

	layout 'login'

	def index
		render layout: "index"
	end

	def getAPIKeys
		serie = Association.find_by key: params[:key]
		api = Array.new
		channel = serie.channel
		field = serie.field
		api_key = ApiKey.find_by channel_id: channel, write_flag: 1
		data = {api_key: api_key.api_key, channel: channel, field: field}
		render :json => data
	end

	def getCapteurTemp
		capteurs = Array.new
		Association.all.each do |e|
			if(e.user_id == current_user.id)
				if(e.channel == ((current_user.id*3)-2))
					capteurs.push(e.field)
				end
			end
		end
		respond_to do |format|
			format.json { render :json => capteurs }
		end
	end


	def key_registrations
		if cookies[:sign_up]
			@user = current_user
			c1 =  @user.channels.create
			c1.id = 3*current_user.id - 2
			c1.name = "Channel 1"
			c1.save
			c1.add_write_api_key
			c2 = @user.channels.create
			c2.id = 3*current_user.id - 1
			c2.name = "Channel 2"
			c2.save
			c2.add_write_api_key
			c3 = @user.channels.create
			c3.id = 3*current_user.id
			c3.name = "Channel 3"
			c3.save
			c3.add_write_api_key
			cookies.delete :sign_up
			render layout: "key_registrations"
		else
			redirect_to '/appli/configuration#/new-channel'
		end
	end

	def channels
		@channels = Array.new
		#@types = Array.new
		@fields = Array.new
		Association.all.each do |e|
			if (e.user_id != nil) & (e.user_id == current_user.id)
				channel = Channel.find(e.channel)
				@channels.push(channel)
				#@types.push(e.name)
				@fields.push(e.field)
			end
		end
		respond_to do |format|
      format.html {	render layout: "configuration" }
      format.json { render :json => {:channels => @channels, :fields => @fields} }
      format.xml { render :xml => @channel.not_social.to_xml(Channel.private_options) }
    end
	end

		def sensors
		@channels = Array.new
		#@types = Array.new
		@fields = Array.new
		Association.all.each do |e|
			if (e.user_id != nil) & (e.user_id == current_user.id)
				channel = Channel.find(e.channel)
				@channels.push(channel)
				#@types.push(e.name)
				@fields.push(e.field)
			end
		end
		respond_to do |format|
      format.html {	render layout: "configuration" }
      format.json { render :json => {:channels => @channels, :fields => @fields} }
      format.xml { render :xml => @channel.not_social.to_xml(Channel.private_options) }
    end
	end

	def newchannel
		message = Array.new
		asso = Association.find_by key: params[:key]
		if asso
			if asso.user_id == nil
				asso.user_id = current_user.id
				if params[:channel] == "1"
					asso.channel = 3*current_user.id - 2
					asso.sensor = params[:channel]
					asso.field = params[:field]
					asso.save
					message[0] = "success"
				elsif params[:channel] == "2"
					asso.channel = 3*current_user.id - 1
					asso.sensor = params[:channel]
					asso.field = params[:field]
					asso.save
					message[0] = "success"
				elsif params[:channel] == "3"
					asso.channel = 3*current_user.id
					asso.sensor = params[:channel]
					asso.field = params[:field]
					asso.save
					message[0] = "success"
				else
					message[0] = "error"
				end
			else
				if asso.user_id == current_user.id
					message[0] = "belongs_to_you"
				else
					message[0] = "belongs_to_another"
				end
			end
		else
			message[0] = "absent"
		end
		respond_to do |format|
			format.json { render :json => message }
		end
	end


	def getkeys
		keys = Array.new(8, "")
		Association.all.each do |e|
			if (e.user_id != nil) & (e.user_id == current_user.id)
				if e.sensor.to_s == params[:channel]
					keys[e.field - 1] = e.key
				end
			end
		end
		print keys
		respond_to do |format|
			format.json {render :json => {:keys => keys}}
		end
	end


	def refresh
		asso = Association.find(params[:id])
		if asso
			respond_to do |format|
				format.json {render :json => {key: asso.key}}
			end
		else
			respond_to do |format|
				format.json {render :json => {key: "error"}}
			end
		end
	end

	def unbind_key
		@message = Array.new
		@message.push("error")
		asso = Association.find_by key: params[:key]
		if asso
			asso.user_id = nil
			asso.field = nil
			asso.sensor = nil
			asso.channel = nil
			asso.save
			@message[0] = "success"
		end
		respond_to do |format|
			format.json {render :json => @message}
		end
	end

	def destroy_session
		redirect_to "/login"
	end

end
