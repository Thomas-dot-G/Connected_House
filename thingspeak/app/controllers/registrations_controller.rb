class RegistrationsController < Devise::RegistrationsController
  include KeyUtilities
  after_filter :add_api_key, :only => :create

  # use defaults from devise
  def new
		cookies[:sign_up] = { value: "XJ-122", expires: 1.hour.from_now }
		super
	end
  def edit; super; end
  def create; super; end



	def after_sign_up_path_for(resource)
		return '/appli/key_registrations'
	end

    # adds an api key to the new user
    def add_api_key
      @user = current_user
      if @user.present?
        @user.api_key = generate_api_key(16, 'user')
        @user.save
      end
    end

end
